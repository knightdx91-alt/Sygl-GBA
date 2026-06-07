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
#include "bn_regular_bg_items_orphanage_interior.h"

// ── Dialogue ──────────────────────────────────────────────────────────────────

static const bn::string_view BROWN_INTRO[] = {
    "Mrs.Brown: There you are.",
    "I was looking for you.",
    "Did you see Bal last night?",
    "He was flying toward the",
    "old mausoleum behind us."
};

// Quiz Q1 choice: A → Waves +2, B → Duality +2

static const bn::string_view BROWN_POST[] = {
    "He returned safely.",
    "More importantly:",
    "A letter arrived today.",
    "You and Bal are accepted",
    "to The Heir's School.",
    "Pack your things tonight.",
    "Verin's cart is at dawn."
};

static const bn::string_view KID_LINES[] = {
    "Kid: Psst! You see Bal?",
    "He flew onto the roof!",
    "Then into the mausoleum.",
    "Think Mrs.Brown knows?"
};

// ── Constructor ───────────────────────────────────────────────────────────────

OrphanageScene::OrphanageScene(GameState& state)
    : _state(state),
      _gen(common::variable_8x16_sprite_font),
      _bg(bn::regular_bg_items::orphanage_interior.create_bg(0, 0))
{
    bn::bg_palettes::set_transparent_color(bn::color(0, 0, 0));

    const bn::regular_bg_map_item& map_item =
        bn::regular_bg_items::orphanage_interior.map_item();
    bn::regular_bg_map_cell floor_cell = map_item.cell(1, 1);
    _floor_tile_index = bn::regular_bg_map_cell_info(floor_cell).tile_index();

    int sx, sy;
    screen_pos(state.tile_x, state.tile_y, sx, sy);
    _player_sprite = bn::sprite_items::player.create_sprite(sx, sy);

    for (int i = 0; i < NPC_COUNT; ++i)
    {
        screen_pos(NPC_TILE_X[i], NPC_TILE_Y[i], sx, sy);
        if (i == 0) _npc_sprite[i] = bn::sprite_items::npc.create_sprite(sx, sy);
        else        _npc_sprite[i] = bn::sprite_items::npc2.create_sprite(sx, sy);
    }
}

// ── Per-frame update ──────────────────────────────────────────────────────────

OrphanageResult OrphanageScene::update()
{
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

    if (_choice_box.is_open())
    {
        int result = _choice_box.update(_gen);
        if (result >= 0)
        {
            _choice_result = result;
            if (result == 0)
                _state.quiz_weights[1] += 2;  // Waves
            else
                _state.quiz_weights[0] += 2;  // Duality
            _text_box.open(_gen, BROWN_POST, 7);
        }
        return OrphanageResult::STAY;
    }

    if (_text_box.is_open())
    {
        update_dialogue();
        return OrphanageResult::STAY;
    }

    // Movement with held-repeat (12-frame initial delay, every 5 frames after)
    int dx = 0, dy = 0;
    if      (bn::keypad::left_held())  { dx = -1; dy =  0; }
    else if (bn::keypad::right_held()) { dx =  1; dy =  0; }
    else if (bn::keypad::up_held())    { dx =  0; dy = -1; }
    else if (bn::keypad::down_held())  { dx =  0; dy =  1; }

    if (dx != 0 || dy != 0)
    {
        bool dir_changed = (dx != _move_dx || dy != _move_dy);
        _move_dx = dx;
        _move_dy = dy;
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
        _move_dx = 0;
        _move_dy = 0;
        _move_timer = 0;
    }

    check_npc_proximity();
    if (_near_npc >= 0 && bn::keypad::a_pressed())
        open_npc_dialogue(_near_npc);

    return OrphanageResult::STAY;
}

// ── Movement & collision ──────────────────────────────────────────────────────

void OrphanageScene::try_move(int dx, int dy)
{
    int nx = _state.tile_x + dx;
    int ny = _state.tile_y + dy;
    if (!tile_walkable(nx, ny)) return;

    for (int i = 0; i < NPC_COUNT; ++i)
        if (NPC_TILE_X[i] == nx && NPC_TILE_Y[i] == ny) return;

    _state.tile_x = nx;
    _state.tile_y = ny;
    int sx, sy;
    screen_pos(nx, ny, sx, sy);
    _player_sprite->set_position(sx, sy);
}

bool OrphanageScene::tile_walkable(int tx, int ty) const
{
    if (tx < 0 || tx >= 32 || ty < 0 || ty >= 32) return false;
    const bn::regular_bg_map_item& map_item =
        bn::regular_bg_items::orphanage_interior.map_item();
    bn::regular_bg_map_cell cell = map_item.cell(tx, ty);
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

void OrphanageScene::open_npc_dialogue(int npc_id)
{
    _active_npc = npc_id;
    _prompt_sprites.clear();
    _near_npc = -1;

    if (npc_id == 0)
        _text_box.open(_gen, BROWN_INTRO, 5);
    else
        _text_box.open(_gen, KID_LINES, 4);
}

void OrphanageScene::update_dialogue()
{
    _text_box.update(_gen);
    if (bn::keypad::a_pressed()) _text_box.advance(_gen);

    if (!_text_box.is_open())
    {
        if (_active_npc == 0 && _choice_result < 0)
        {
            _choice_box.open(_gen,
                bn::string_view("What did you do?"),
                bn::string_view("You warned him"),
                bn::string_view("You watched"));
        }
        else if (_active_npc == 0 && _choice_result >= 0)
        {
            _brown_complete = true;
            _active_npc = -1;
        }
        else
        {
            _active_npc = -1;
        }
    }
}
