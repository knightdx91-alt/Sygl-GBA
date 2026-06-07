#include "overworld_scene.h"
#include "item_data.h"
#include "character.h"
#include "bn_keypad.h"
#include "bn_bg_palettes.h"
#include "bn_color.h"
#include "bn_string.h"
#include "bn_regular_bg_map_cell_info.h"
#include "common_variable_8x16_sprite_font.h"
#include "bn_sprite_items_player.h"
#include "bn_sprite_items_npc.h"
#include "bn_sprite_items_npc2.h"
#include "bn_sprite_items_npc3.h"
#include "bn_sprite_items_enemy_bandit.h"
#include "bn_sprite_items_enemy_wolf.h"
#include "bn_regular_bg_items_room.h"

// ── Static layout data ────────────────────────────────────────────────────────

// ── NPC dialogue data ─────────────────────────────────────────────────────────

static const bn::string_view NPC0_INTRO[] = {
    "Soren: Hey there, newcomer.",
    "I work the desk at Dawn Hall.",
    "Have you heard about us?"
};
static const bn::string_view NPC0_YES[] = {
    "Soren: We post missions for",
    "those with sygls. Find us",
    "when you're ready to join."
};
static const bn::string_view NPC0_NO[] = {
    "Soren: Dawn Hall is the",
    "adventurers guild in",
    "New Spring. Come back soon."
};

static const bn::string_view NPC1_LINES[] = {
    "Kim: Watch yourself out",
    "there. Bandits have been",
    "spotted near the east road.",
    "And wolves in the woods.",
    "Stay sharp."
};
static const bn::string_view NPC2_LINES[] = {
    "Vendor: Fresh stock today!",
    "Come back when I have",
    "my inventory set up.",
    "(Shop system: Phase 3)"
};

// ── Constructor ───────────────────────────────────────────────────────────────

OverworldScene::OverworldScene(GameState& state)
    : _state(state),
      _gen(common::variable_8x16_sprite_font),
      _bg(bn::regular_bg_items::room.create_bg(0, 0))
{
    bn::bg_palettes::set_transparent_color(bn::color(2, 1, 1));

    // Determine walkable tile index from a known interior cell
    const bn::regular_bg_map_item& map_item = bn::regular_bg_items::room.map_item();
    bn::regular_bg_map_cell floor_cell = map_item.cell(1, 1);
    _floor_tile_index = bn::regular_bg_map_cell_info(floor_cell).tile_index();

    // Player sprite
    int sx, sy;
    screen_pos(state.tile_x, state.tile_y, sx, sy);
    _player_sprite = bn::sprite_items::player.create_sprite(sx, sy);

    // NPC sprites
    for (int i = 0; i < NPC_COUNT; ++i)
    {
        screen_pos(NPC_TILE_X[i], NPC_TILE_Y[i], sx, sy);
        if (i == 0)      _npc_sprite[i] = bn::sprite_items::npc.create_sprite(sx, sy);
        else if (i == 1) _npc_sprite[i] = bn::sprite_items::npc2.create_sprite(sx, sy);
        else             _npc_sprite[i] = bn::sprite_items::npc3.create_sprite(sx, sy);
    }

    // Enemy sprites (only for alive enemies)
    for (int i = 0; i < ENEMY_COUNT; ++i)
    {
        if (state.enemy_alive[i])
        {
            screen_pos(ENEMY_TILE_X[i], ENEMY_TILE_Y[i], sx, sy);
            if (ENEMY_TYPE[i] == EnemyType::BANDIT)
                _enemy_sprite[i] = bn::sprite_items::enemy_bandit.create_sprite(sx, sy);
            else
                _enemy_sprite[i] = bn::sprite_items::enemy_wolf.create_sprite(sx, sy);
        }
    }
}

// ── Per-frame update ──────────────────────────────────────────────────────────

OverworldResult OverworldScene::update()
{
    // Dialogue has priority over movement
    if (_choice_box.is_open())
    {
        int result = _choice_box.update(_gen);
        if (result >= 0)
        {
            _choice_result = result;
            if (_active_npc == 0)
                _text_box.open(_gen,
                    result == 0 ? NPC0_YES : NPC0_NO,
                    3);
        }
        return OverworldResult::STAY;
    }

    if (_text_box.is_open())
    {
        update_dialogue();
        return OverworldResult::STAY;
    }

    // FF-style menu
    if (bn::keypad::start_pressed())
    {
        if (_paused) close_menu();
        else         open_menu();
        return OverworldResult::STAY;
    }
    if (_paused)
    {
        // Sub-screen active — B returns to main menu
        if (_menu_sub != MenuSub::NONE)
        {
            if (bn::keypad::b_pressed())
            {
                _menu_sub = MenuSub::NONE;
                _sub_cursor = 0;
                render_menu();
            }
            else if (_menu_sub == MenuSub::ITEMS || _menu_sub == MenuSub::MAGIC)
            {
                // scroll sub-list
                int max_items = (_menu_sub == MenuSub::ITEMS)
                    ? _state.party.inventory.count
                    : _state.party.player().spell_count;
                if (bn::keypad::up_pressed()   && _sub_cursor > 0)          { _sub_cursor--; render_menu(); }
                if (bn::keypad::down_pressed()  && _sub_cursor < max_items-1) { _sub_cursor++; render_menu(); }
            }
            return OverworldResult::STAY;
        }

        // Main menu navigation
        static constexpr int MENU_OPTIONS = 5;
        if (bn::keypad::up_pressed())   { _menu_cursor = (_menu_cursor + MENU_OPTIONS - 1) % MENU_OPTIONS; render_menu(); }
        if (bn::keypad::down_pressed()) { _menu_cursor = (_menu_cursor + 1) % MENU_OPTIONS;                render_menu(); }
        if (bn::keypad::b_pressed() || (bn::keypad::a_pressed() && _menu_cursor == 4))
        {
            close_menu();
            return OverworldResult::STAY;
        }
        if (bn::keypad::a_pressed())
        {
            if (_menu_cursor == 0) { _menu_sub = MenuSub::ITEMS;  _sub_cursor = 0; render_menu(); }
            if (_menu_cursor == 1) { _menu_sub = MenuSub::MAGIC;  _sub_cursor = 0; render_menu(); }
            if (_menu_cursor == 2) { _menu_sub = MenuSub::STATUS; render_menu(); }
            if (_menu_cursor == 3)
            {
                // Save placeholder
                _menu_sub = MenuSub::STATUS; // reuse STATUS area to show "Saved!"
                render_menu();
            }
        }
        return OverworldResult::STAY;
    }

    // Movement with held-repeat (12 frame initial delay, then every 5 frames)
    int dx = 0, dy = 0;
    if      (bn::keypad::left_held())  { dx = -1; dy =  0; }
    else if (bn::keypad::right_held()) { dx =  1; dy =  0; }
    else if (bn::keypad::up_held())    { dx =  0; dy = -1; }
    else if (bn::keypad::down_held())  { dx =  0; dy =  1; }

    if (dx != 0 || dy != 0)
    {
        bool direction_changed = (dx != _move_dx || dy != _move_dy);
        _move_dx = dx;
        _move_dy = dy;
        if (direction_changed)
        {
            _move_timer = 0;
            try_move(dx, dy);
        }
        else
        {
            _move_timer++;
            if (_move_timer >= 12 && (_move_timer - 12) % 5 == 0)
                try_move(dx, dy);
        }
    }
    else
    {
        _move_dx = 0;
        _move_dy = 0;
        _move_timer = 0;
    }

    // Check if we stepped on an enemy
    OverworldResult contact = check_enemy_contact();
    if (contact != OverworldResult::STAY) return contact;

    // NPC proximity + interaction
    check_npc_proximity();
    if (_near_npc >= 0 && bn::keypad::a_pressed())
        open_npc_dialogue(_near_npc);

    // HUD refresh
    int cur_hp = _state.party.player().hp;
    if (cur_hp != _last_hp)
    {
        _last_hp = cur_hp;
        render_hud();
    }

    return OverworldResult::STAY;
}

// ── Movement & collision ──────────────────────────────────────────────────────

void OverworldScene::try_move(int dx, int dy)
{
    int nx = _state.tile_x + dx;
    int ny = _state.tile_y + dy;
    if (!tile_walkable(nx, ny)) return;

    // Don't walk into NPC tiles
    for (int i = 0; i < NPC_COUNT; ++i)
        if (NPC_TILE_X[i] == nx && NPC_TILE_Y[i] == ny) return;

    _state.tile_x = nx;
    _state.tile_y = ny;
    int sx, sy;
    screen_pos(nx, ny, sx, sy);
    _player_sprite->set_position(sx, sy);
}

bool OverworldScene::tile_walkable(int tx, int ty) const
{
    if (tx < 0 || tx >= 32 || ty < 0 || ty >= 32) return false;
    const bn::regular_bg_map_item& map_item = bn::regular_bg_items::room.map_item();
    bn::regular_bg_map_cell cell = map_item.cell(tx, ty);
    return bn::regular_bg_map_cell_info(cell).tile_index() == _floor_tile_index;
}

void OverworldScene::screen_pos(int tx, int ty, int& sx, int& sy) const
{
    sx = tx * 8 - 124;
    sy = ty * 8 - 124;
}

// ── NPC proximity ─────────────────────────────────────────────────────────────

void OverworldScene::check_npc_proximity()
{
    int near = -1;
    for (int i = 0; i < NPC_COUNT; ++i)
    {
        int dx = _state.tile_x - NPC_TILE_X[i];
        int dy = _state.tile_y - NPC_TILE_Y[i];
        if ((dx < 0 ? -dx : dx) + (dy < 0 ? -dy : dy) <= 1)
        {
            near = i;
            break;
        }
    }

    if (near != _near_npc)
    {
        _near_npc = near;
        _prompt_sprites.clear();
        if (_near_npc >= 0)
        {
            int sx, sy;
            screen_pos(NPC_TILE_X[_near_npc], NPC_TILE_Y[_near_npc], sx, sy);
            _gen.set_center_alignment();
            _gen.generate(sx, sy - 18, bn::string_view("Press A"), _prompt_sprites);
        }
    }
}

void OverworldScene::open_npc_dialogue(int npc_id)
{
    _active_npc = npc_id;
    _prompt_sprites.clear();
    _near_npc = -1;

    if (npc_id == 0)
        _text_box.open(_gen, NPC0_INTRO, 3);
    else if (npc_id == 1)
        _text_box.open(_gen, NPC1_LINES, 5);
    else
        _text_box.open(_gen, NPC2_LINES, 4);
}

void OverworldScene::update_dialogue()
{
    _text_box.update(_gen);
    if (bn::keypad::a_pressed()) _text_box.advance(_gen);

    // After NPC0's intro closes, show the choice
    if (!_text_box.is_open() && _active_npc == 0 && _choice_result < 0)
        _choice_box.open(_gen,
            bn::string_view("Dawn Hall?"),
            bn::string_view("Tell me more"),
            bn::string_view("Maybe later"));
}

// ── Enemy contact ─────────────────────────────────────────────────────────────

OverworldResult OverworldScene::check_enemy_contact()
{
    for (int i = 0; i < ENEMY_COUNT; ++i)
    {
        if (!_state.enemy_alive[i]) continue;
        if (_state.tile_x == ENEMY_TILE_X[i] && _state.tile_y == ENEMY_TILE_Y[i])
        {
            _pending_enemy = ENEMY_TYPE[i];
            return OverworldResult::COMBAT;
        }
    }
    return OverworldResult::STAY;
}

// ── HUD ───────────────────────────────────────────────────────────────────────

void OverworldScene::render_hud()
{
    _hud_sprites.clear();
    _gen.set_left_alignment();
    bn::string<24> hud;
    hud.append(bn::string_view("HP "));
    // Simple number append
    int hp = _state.party.player().hp;
    if (hp < 0) hp = 0;
    if (hp >= 100) hud.push_back('0' + hp / 100);
    if (hp >= 10)  hud.push_back('0' + (hp / 10) % 10);
    hud.push_back('0' + hp % 10);
    hud.append(bn::string_view("/"));
    int hm = _state.party.player().eff_hp_max();
    if (hm >= 100) hud.push_back('0' + hm / 100);
    if (hm >= 10)  hud.push_back('0' + (hm / 10) % 10);
    hud.push_back('0' + hm % 10);
    _gen.generate(-116, -72, hud, _hud_sprites);
}

// ── FF-style menu ─────────────────────────────────────────────────────────────

static void append_int(bn::string<32>& s, int v)
{
    if (v < 0) { s.push_back('-'); v = -v; }
    if (v >= 100) s.push_back('0' + v / 100);
    if (v >= 10)  s.push_back('0' + (v / 10) % 10);
    s.push_back('0' + v % 10);
}

void OverworldScene::open_menu()
{
    _paused      = true;
    _menu_cursor = 0;
    _menu_sub    = MenuSub::NONE;
    _sub_cursor  = 0;
    render_menu();
}

void OverworldScene::close_menu()
{
    _paused = false;
    _menu_sprites.clear();
}

// Layout: left panel (char info) x=-116..-10, right panel (menu) x=10..116
// y grid: -72,-56,-40,-24,-8,8,24,40,56,72  (16px steps)
void OverworldScene::render_menu()
{
    _menu_sprites.clear();
    const Character& pc  = _state.party.player();
    const Inventory& inv = _state.party.inventory;

    // ── Header ──────────────────────────────────────────────────────
    _gen.set_center_alignment();
    _gen.generate(0, -72, bn::string_view("* MENU *"), _menu_sprites);

    if (_menu_sub == MenuSub::NONE)
    {
        // ── Left panel: character card ───────────────────────────────
        _gen.set_left_alignment();

        bn::string<32> ln;
        ln.append(bn::string_view(pc.name));
        _gen.generate(-116, -52, ln, _menu_sprites);

        ln.clear();
        ln.append(bn::string_view("Lv "));
        append_int(ln, pc.level);
        _gen.generate(-116, -36, ln, _menu_sprites);

        ln.clear();
        ln.append(bn::string_view("HP "));
        append_int(ln, pc.hp);
        ln.push_back('/');
        append_int(ln, pc.eff_hp_max());
        _gen.generate(-116, -20, ln, _menu_sprites);

        ln.clear();
        ln.append(bn::string_view("MP "));
        append_int(ln, pc.mp);
        ln.push_back('/');
        append_int(ln, pc.eff_mp_max());
        _gen.generate(-116, -4, ln, _menu_sprites);

        ln.clear();
        ln.append(bn::string_view("XP "));
        append_int(ln, pc.xp);
        ln.push_back('/');
        append_int(ln, pc.xp_next);
        _gen.generate(-116, 12, ln, _menu_sprites);

        // ── Right panel: menu options ────────────────────────────────
        static const bn::string_view OPTIONS[] = {
            "Items", "Magic", "Status", "Save", "Close"
        };
        for (int i = 0; i < 5; ++i)
        {
            ln.clear();
            ln.append(bn::string_view(i == _menu_cursor ? "> " : "  "));
            for (char c : OPTIONS[i]) ln.push_back(c);
            _gen.set_left_alignment();
            _gen.generate(20, -52 + i * 20, ln, _menu_sprites);
        }

        _gen.set_center_alignment();
        _gen.generate(0, 68, bn::string_view("A:Select  B:Close"), _menu_sprites);
    }
    else if (_menu_sub == MenuSub::ITEMS)
    {
        // ── Items sub-screen ─────────────────────────────────────────
        _gen.set_center_alignment();
        _gen.generate(0, -52, bn::string_view("[ ITEMS ]"), _menu_sprites);

        _gen.set_left_alignment();
        if (inv.count == 0)
        {
            _gen.set_center_alignment();
            _gen.generate(0, 0, bn::string_view("(Empty)"), _menu_sprites);
        }
        else
        {
            int shown = inv.count < 7 ? inv.count : 7;
            for (int i = 0; i < shown; ++i)
            {
                const InventorySlot& slot = inv.slots[i];
                if (slot.id == ItemId::NONE) continue;
                bn::string<32> ln;
                ln.append(bn::string_view(i == _sub_cursor ? "> " : "  "));
                const ItemDef& it = item_def(slot.id);
                ln.append(bn::string_view(it.name));
                ln.append(bn::string_view("  x"));
                append_int(ln, slot.qty);
                _gen.generate(-116, -36 + i * 14, ln, _menu_sprites);
            }
        }
        _gen.set_center_alignment();
        _gen.generate(0, 68, bn::string_view("B:Back"), _menu_sprites);
    }
    else if (_menu_sub == MenuSub::MAGIC)
    {
        // ── Magic sub-screen ─────────────────────────────────────────
        _gen.set_center_alignment();
        _gen.generate(0, -52, bn::string_view("[ MAGIC ]"), _menu_sprites);

        _gen.set_left_alignment();
        if (pc.spell_count == 0)
        {
            _gen.set_center_alignment();
            _gen.generate(0, 0, bn::string_view("(No spells)"), _menu_sprites);
        }
        else
        {
            for (int i = 0; i < pc.spell_count; ++i)
            {
                const SpellDef& sp = spell_def(pc.known_spells[i]);
                bn::string<32> ln;
                ln.append(bn::string_view(i == _sub_cursor ? "> " : "  "));
                ln.append(bn::string_view(sp.name));
                ln.append(bn::string_view("  "));
                append_int(ln, sp.mp_cost);
                ln.append(bn::string_view("MP"));
                _gen.generate(-116, -36 + i * 16, ln, _menu_sprites);
            }
        }
        _gen.set_center_alignment();
        _gen.generate(0, 68, bn::string_view("B:Back"), _menu_sprites);
    }
    else if (_menu_sub == MenuSub::STATUS)
    {
        // ── Status sub-screen ────────────────────────────────────────
        _gen.set_center_alignment();
        _gen.generate(0, -52, bn::string_view("[ STATUS ]"), _menu_sprites);

        _gen.set_left_alignment();
        bn::string<32> ln;

        ln.clear(); ln.append(bn::string_view(pc.name));
        ln.append(bn::string_view("  Lv ")); append_int(ln, pc.level);
        _gen.generate(-116, -32, ln, _menu_sprites);

        ln.clear(); ln.append(bn::string_view("ATK ")); append_int(ln, pc.eff_atk());
        ln.append(bn::string_view("   DEF ")); append_int(ln, pc.eff_def());
        _gen.generate(-116, -14, ln, _menu_sprites);

        ln.clear(); ln.append(bn::string_view("SPD ")); append_int(ln, pc.eff_spd());
        ln.append(bn::string_view("   MAG ")); append_int(ln, pc.eff_mag());
        _gen.generate(-116, 4, ln, _menu_sprites);

        ln.clear(); ln.append(bn::string_view("HP  ")); append_int(ln, pc.hp);
        ln.push_back('/'); append_int(ln, pc.eff_hp_max());
        _gen.generate(-116, 22, ln, _menu_sprites);

        ln.clear(); ln.append(bn::string_view("MP  ")); append_int(ln, pc.mp);
        ln.push_back('/'); append_int(ln, pc.eff_mp_max());
        _gen.generate(-116, 40, ln, _menu_sprites);

        _gen.set_center_alignment();
        _gen.generate(0, 68, bn::string_view("B:Back"), _menu_sprites);
    }
}

void OverworldScene::render_sub_items()  {}
void OverworldScene::render_sub_magic()  {}
void OverworldScene::render_sub_status() {}
