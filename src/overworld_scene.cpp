#include "overworld_scene.h"
#include "bn_keypad.h"
#include "bn_bg_palettes.h"
#include "bn_math.h"
#include "common_variable_8x16_sprite_font.h"

static const bn::string_view NPC_DIALOGUE[] = {
    "Hello, traveler!",
    "Welcome to the test room.",
    "The Heir's School awaits",
    "those brave enough to seek it.",
    "Development: Phase 1 complete.",
    "Stay tuned for more!"
};
static constexpr int NPC_DIALOGUE_COUNT = 6;

OverworldScene::OverworldScene()
    : _gen(common::variable_8x16_sprite_font),
      _px(0), _py(0)
{
    bn::bg_palettes::set_transparent_color(bn::color(2, 4, 8));

    // Draw player "@" at starting position
    _gen.set_center_alignment();
    _gen.generate(_px.integer(), _py.integer(), bn::string_view("@"), _player_sprites);

    // Draw NPC "N" character
    _gen.generate(NPC_X, NPC_Y, bn::string_view("N"), _npc_sprites);
}

void OverworldScene::update()
{
    if (_text_box.is_open())
    {
        _text_box.update(_gen);
        if (bn::keypad::a_pressed())
            _text_box.advance(_gen);
        return;
    }

    if (_paused)
    {
        if (bn::keypad::start_pressed() || bn::keypad::a_pressed())
        {
            _paused = false;
            _pause_sprites.clear();
        }
        return;
    }

    if (bn::keypad::start_pressed())
    {
        _paused = true;
        _pause_sprites.clear();
        _gen.set_center_alignment();
        _gen.generate(0, 0, bn::string_view("-- PAUSED --"), _pause_sprites);
        _gen.generate(0, 20, bn::string_view("Press Start to resume"), _pause_sprites);
        return;
    }

    update_player();
    update_npc_interaction();
}

void OverworldScene::update_player()
{
    bn::fixed nx = _px;
    bn::fixed ny = _py;

    if (bn::keypad::left_pressed())  nx -= STEP;
    if (bn::keypad::right_pressed()) nx += STEP;
    if (bn::keypad::up_pressed())    ny -= STEP;
    if (bn::keypad::down_pressed())  ny += STEP;

    // Clamp to room bounds
    if (nx < LEFT_BOUND)  nx = LEFT_BOUND;
    if (nx > RIGHT_BOUND) nx = RIGHT_BOUND;
    if (ny < TOP_BOUND)   ny = TOP_BOUND;
    if (ny > BOT_BOUND)   ny = BOT_BOUND;

    if (nx != _px || ny != _py)
    {
        _px = nx;
        _py = ny;
        _player_sprites.clear();
        _gen.set_center_alignment();
        _gen.generate(_px.integer(), _py.integer(), bn::string_view("@"), _player_sprites);
    }
}

void OverworldScene::update_npc_interaction()
{
    int dx = (_px - NPC_X).integer();
    int dy = (_py - NPC_Y).integer();
    int dist = (dx < 0 ? -dx : dx) + (dy < 0 ? -dy : dy);

    bool near_npc = dist <= INTERACT_DIST;

    if (near_npc != _prompt_visible)
    {
        _prompt_visible = near_npc;
        _prompt_sprites.clear();
        if (_prompt_visible)
        {
            _gen.set_center_alignment();
            _gen.generate(NPC_X, NPC_Y - 20, bn::string_view("Press A"), _prompt_sprites);
        }
    }

    if (near_npc && bn::keypad::a_pressed())
    {
        _prompt_sprites.clear();
        _prompt_visible = false;
        _text_box.open(_gen, NPC_DIALOGUE, NPC_DIALOGUE_COUNT);
    }
}
