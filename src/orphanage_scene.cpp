#include "orphanage_scene.h"
#include "bn_keypad.h"
#include "bn_bg_palettes.h"
#include "bn_color.h"
#include "bn_string.h"
#include "bn_regular_bg_map_cell_info.h"
#include "common_variable_8x16_sprite_font.h"
#include "bn_sprite_items_player.h"
#include "bn_sprite_items_npc.h"
#include "bn_sprite_items_npc2.h"
#include "bn_regular_bg_items_orphanage_corridor.h"
#include "bn_regular_bg_items_orphanage_office.h"
#include "bn_regular_bg_items_orphanage_rooftop.h"

// ── Dialogue ──────────────────────────────────────────────────────────────────

// Rooftop: Bal flies to the mausoleum
static const bn::string_view CUTSCENE_BAL[] = {
    "You step to the parapet edge.",
    "Across the grounds, in the dark,",
    "Bal stands on the back roof.",
    "Something ripples under his feet.",
    "A carpet. Enchanted.",
    "He rises slowly, then faster.",
    "Across to the mausoleum.",
    "It has no real roof—just sky.",
    "He drops inside. Disappears.",
    "Whatever is in there, he found it."
};

// Corridor kid
static const bn::string_view KID_LINES[] = {
    "Kid: Have you seen Bal?",
    "He snuck up to the roof again.",
    "Mrs.Brown'll lose it if she finds out.",
    "He does this every night."
};

// Mrs. Brown — Quiz Q1
// The player stayed quiet during Simeon's bullying
static const bn::string_view BROWN_INTRO[] = {
    "Mrs.Brown: Sit down, please.",
    "I want to talk about Simeon.",
    "You saw what those boys did.",
    "Why did you stay quiet?"
};

// Choice A: "I didn't want it to escalate." → Waves +2
// Choice B: "I was choosing the right moment." → Tempus +2

static const bn::string_view BROWN_POST[] = {
    "Hmm. I understand.",
    "Now — I have news for you.",
    "A letter arrived this morning.",
    "You and Bal have been accepted",
    "to The Heir's School.",
    "Pack your things tonight.",
    "Verin's cart leaves at dawn."
};

// ── Constructor ───────────────────────────────────────────────────────────────

OrphanageScene::OrphanageScene(GameState& state)
    : _state(state),
      _gen(common::variable_8x16_sprite_font)
{
    bn::bg_palettes::set_transparent_color(bn::color(0, 0, 0));

    // Start in corridor, player at centre
    _state.tile_x = 15;
    _state.tile_y = 16;

    int sx, sy;
    screen_pos(_state.tile_x, _state.tile_y, sx, sy);
    _player_sprite = bn::sprite_items::player.create_sprite(sx, sy);

    switch_room(ORoom::CORRIDOR, _state.tile_x, _state.tile_y);
}

// ── Per-frame update ──────────────────────────────────────────────────────────

OrphanageResult OrphanageScene::update()
{
    // Leave countdown after Mrs. Brown's full dialogue
    if (_brown_complete)
    {
        _leave_timer++;
        if (_leave_timer > 90)
        {
            _state.orphanage_done = true;
            return OrphanageResult::LEAVE_FOR_SCHOOL;
        }
        return OrphanageResult::STAY;
    }

    // Choice box
    if (_choice_box.is_open())
    {
        int result = _choice_box.update(_gen);
        if (result >= 0)
        {
            _choice_result = result;
            if (result == 0)
                _state.quiz_weights[1] += 2;  // Waves — empathetic
            else
                _state.quiz_weights[3] += 2;  // Tempus — tactical
            _text_box.open(_gen, BROWN_POST, 7);
        }
        return OrphanageResult::STAY;
    }

    // Text box (dialogue or cutscene)
    if (_text_box.is_open())
    {
        update_dialogue();
        return OrphanageResult::STAY;
    }

    // Movement
    int dx = 0, dy = 0;
    if      (bn::keypad::left_held())  { dx = -1; }
    else if (bn::keypad::right_held()) { dx =  1; }
    else if (bn::keypad::up_held())    { dy = -1; }
    else if (bn::keypad::down_held())  { dy =  1; }

    if (dx != 0 || dy != 0)
    {
        bool dir_changed = (dx != _move_dx || dy != _move_dy);
        _move_dx = dx; _move_dy = dy;
        if (dir_changed)
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
        _move_dx = 0; _move_dy = 0; _move_timer = 0;
    }

    check_transitions();
    check_npc_proximity();
    if (_near_npc >= 0 && bn::keypad::a_pressed())
        open_npc_dialogue(_near_npc);

    return OrphanageResult::STAY;
}

// ── Room switching ────────────────────────────────────────────────────────────

void OrphanageScene::switch_room(ORoom to, int px, int py)
{
    // Clear NPCs and prompt
    for (int i = 0; i < MAX_NPCS; ++i) _npc_sprite[i].reset();
    _prompt_sprites.clear();
    _near_npc  = -1;
    _npc_count = 0;
    _active_npc   = -1;
    _choice_result = -1;
    _cutscene_active = false;

    _room = to;
    _state.tile_x = px;
    _state.tile_y = py;

    // Swap background
    _bg.reset();
    const bn::regular_bg_map_item* map_item_ptr = nullptr;

    if (to == ORoom::CORRIDOR)
    {
        _bg = bn::regular_bg_items::orphanage_corridor.create_bg(0, 0);
        map_item_ptr = &bn::regular_bg_items::orphanage_corridor.map_item();

        // Kid NPC in the corridor
        _npc_count = 1;
        _npc_tile_x[0] = 22; _npc_tile_y[0] = 12;
        int sx, sy;
        screen_pos(_npc_tile_x[0], _npc_tile_y[0], sx, sy);
        _npc_sprite[0] = bn::sprite_items::npc2.create_sprite(sx, sy);
    }
    else if (to == ORoom::OFFICE)
    {
        _bg = bn::regular_bg_items::orphanage_office.create_bg(0, 0);
        map_item_ptr = &bn::regular_bg_items::orphanage_office.map_item();

        // Mrs. Brown behind her desk
        _npc_count = 1;
        _npc_tile_x[0] = 14; _npc_tile_y[0] = 12;
        int sx, sy;
        screen_pos(_npc_tile_x[0], _npc_tile_y[0], sx, sy);
        _npc_sprite[0] = bn::sprite_items::npc.create_sprite(sx, sy);
    }
    else // ROOFTOP
    {
        _bg = bn::regular_bg_items::orphanage_rooftop.create_bg(0, 0);
        map_item_ptr = &bn::regular_bg_items::orphanage_rooftop.map_item();
        // No NPCs on rooftop
    }

    // Recompute walkable tile index for new map
    bn::regular_bg_map_cell floor_cell = map_item_ptr->cell(2, 2);
    _floor_tile_index = bn::regular_bg_map_cell_info(floor_cell).tile_index();

    // Reposition player sprite
    int sx, sy;
    screen_pos(px, py, sx, sy);
    _player_sprite->set_position(sx, sy);
}

// ── Transitions ───────────────────────────────────────────────────────────────

void OrphanageScene::check_transitions()
{
    int tx = _state.tile_x;
    int ty = _state.tile_y;

    if (_room == ORoom::CORRIDOR)
    {
        // Left edge opening (rows 13-14) → office
        if (tx == 0 && (ty == 13 || ty == 14))
            switch_room(ORoom::OFFICE, 28, 14);
        // Top edge opening (cols 14-16) → rooftop
        else if (ty == 0 && (tx == 14 || tx == 15 || tx == 16))
            switch_room(ORoom::ROOFTOP, 15, 28);
    }
    else if (_room == ORoom::OFFICE)
    {
        // Right edge opening (rows 14-15) → corridor
        if (tx == 31 && (ty == 14 || ty == 15))
            switch_room(ORoom::CORRIDOR, 1, 14);
    }
    else if (_room == ORoom::ROOFTOP)
    {
        // Bottom edge opening (cols 13-15) → corridor
        if (ty == 31 && (tx == 13 || tx == 14 || tx == 15))
            switch_room(ORoom::CORRIDOR, 15, 1);
        // North edge → Bal cutscene (only once, only if not already seen)
        else if (ty <= 3 && !_rooftop_seen && !_cutscene_active)
        {
            _cutscene_active = true;
            _text_box.open(_gen, CUTSCENE_BAL, 10);
        }
    }
}

// ── Movement & collision ──────────────────────────────────────────────────────

void OrphanageScene::try_move(int dx, int dy)
{
    int nx = _state.tile_x + dx;
    int ny = _state.tile_y + dy;
    if (!tile_walkable(nx, ny)) return;

    for (int i = 0; i < _npc_count; ++i)
        if (_npc_tile_x[i] == nx && _npc_tile_y[i] == ny) return;

    _state.tile_x = nx;
    _state.tile_y = ny;
    int sx, sy;
    screen_pos(nx, ny, sx, sy);
    _player_sprite->set_position(sx, sy);
}

bool OrphanageScene::tile_walkable(int tx, int ty) const
{
    if (tx < 0 || tx >= 32 || ty < 0 || ty >= 32) return false;

    const bn::regular_bg_map_item* map_item_ptr = nullptr;
    if (_room == ORoom::CORRIDOR)
        map_item_ptr = &bn::regular_bg_items::orphanage_corridor.map_item();
    else if (_room == ORoom::OFFICE)
        map_item_ptr = &bn::regular_bg_items::orphanage_office.map_item();
    else
        map_item_ptr = &bn::regular_bg_items::orphanage_rooftop.map_item();

    bn::regular_bg_map_cell cell = map_item_ptr->cell(tx, ty);
    return bn::regular_bg_map_cell_info(cell).tile_index() == _floor_tile_index;
}

void OrphanageScene::screen_pos(int tx, int ty, int& sx, int& sy) const
{
    sx = tx * 8 - 124;
    sy = ty * 8 - 124;
}

// ── NPC proximity ─────────────────────────────────────────────────────────────

void OrphanageScene::check_npc_proximity()
{
    int near = -1;
    for (int i = 0; i < _npc_count; ++i)
    {
        int dx = _state.tile_x - _npc_tile_x[i];
        int dy = _state.tile_y - _npc_tile_y[i];
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
            screen_pos(_npc_tile_x[_near_npc], _npc_tile_y[_near_npc], sx, sy);
            _gen.set_center_alignment();
            _gen.generate(sx, sy - 18, bn::string_view("Press A"), _prompt_sprites);
        }
    }
}

void OrphanageScene::open_npc_dialogue(int npc_id)
{
    _active_npc = npc_id;
    _prompt_sprites.clear();
    _near_npc = -1;

    if (_room == ORoom::CORRIDOR)
    {
        // Only one NPC: the kid
        _text_box.open(_gen, KID_LINES, 4);
    }
    else if (_room == ORoom::OFFICE)
    {
        // Mrs. Brown — quiz Q1
        _text_box.open(_gen, BROWN_INTRO, 4);
    }
}

void OrphanageScene::update_dialogue()
{
    _text_box.update(_gen);
    if (bn::keypad::a_pressed()) _text_box.advance(_gen);

    if (_text_box.is_open()) return;

    if (_cutscene_active)
    {
        // Rooftop Bal cutscene finished
        _cutscene_active = false;
        _rooftop_seen    = true;
        return;
    }

    if (_room == ORoom::OFFICE && _active_npc == 0)
    {
        if (_choice_result < 0)
        {
            // BROWN_INTRO finished — show Quiz Q1
            _choice_box.open(_gen,
                bn::string_view("Why stay quiet?"),
                bn::string_view("Didn't want worse"),
                bn::string_view("Right moment"));
        }
        else
        {
            // BROWN_POST finished — scene done
            _brown_complete = true;
            _active_npc = -1;
        }
    }
    else
    {
        _active_npc = -1;
    }
}
