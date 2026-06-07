#include "overworld_scene.h"
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

constexpr int OverworldScene::NPC_TILE_X[NPC_COUNT]    = {  8, 16, 24 };
constexpr int OverworldScene::NPC_TILE_Y[NPC_COUNT]    = { 12,  8, 12 };

constexpr EnemyType OverworldScene::ENEMY_TYPE[ENEMY_COUNT] = {
    EnemyType::BANDIT, EnemyType::WOLF };

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
    bn::bg_palettes::set_transparent_color(bn::color(2, 2, 4));

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
                    result == 0 ? 3 : 3);
        }
        return OverworldResult::STAY;
    }

    if (_text_box.is_open())
    {
        update_dialogue();
        return OverworldResult::STAY;
    }

    // Movement
    if (bn::keypad::left_pressed())  try_move(-1,  0);
    if (bn::keypad::right_pressed()) try_move( 1,  0);
    if (bn::keypad::up_pressed())    try_move( 0, -1);
    if (bn::keypad::down_pressed())  try_move( 0,  1);

    // Check if we stepped on an enemy
    OverworldResult contact = check_enemy_contact();
    if (contact != OverworldResult::STAY) return contact;

    // NPC proximity + interaction
    check_npc_proximity();
    if (_near_npc >= 0 && bn::keypad::a_pressed())
        open_npc_dialogue(_near_npc);

    // HUD refresh
    if (_state.hp != _last_hp)
    {
        _last_hp = _state.hp;
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
    _player_sprite.set_position(sx, sy);
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
    int hp = _state.hp;
    if (hp < 0) hp = 0;
    if (hp >= 100) hud.push_back('0' + hp / 100);
    if (hp >= 10)  hud.push_back('0' + (hp / 10) % 10);
    hud.push_back('0' + hp % 10);
    hud.append(bn::string_view("/"));
    int hm = _state.hp_max;
    if (hm >= 100) hud.push_back('0' + hm / 100);
    if (hm >= 10)  hud.push_back('0' + (hm / 10) % 10);
    hud.push_back('0' + hm % 10);
    _gen.generate(-116, -72, hud, _hud_sprites);
}
