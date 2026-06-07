#include "title_scene.h"
#include "bn_keypad.h"
#include "bn_bg_palettes.h"
#include "common_variable_8x16_sprite_font.h"

TitleScene::TitleScene(bool has_save)
    : _gen(common::variable_8x16_sprite_font),
      _has_save(has_save),
      _cursor(0),
      _done(false),
      _chose_new_game(true)
{
    bn::bg_palettes::set_transparent_color(bn::color(2, 4, 8));
    render(_gen);
}

void TitleScene::update()
{
    bool changed = false;

    if (bn::keypad::down_pressed() && _has_save)
    {
        _cursor = (_cursor + 1) % 2;
        changed = true;
    }
    else if (bn::keypad::up_pressed() && _has_save)
    {
        _cursor = (_cursor - 1 + 2) % 2;
        changed = true;
    }

    if (bn::keypad::a_pressed())
    {
        _chose_new_game = (_cursor == 0);
        _done = true;
        return;
    }

    if (changed) render(_gen);
}

void TitleScene::render(bn::sprite_text_generator& gen)
{
    _sprites.clear();
    gen.set_center_alignment();

    gen.generate(0, -50, bn::string_view("SYGL"), _sprites);
    gen.generate(0, -32, bn::string_view("A Magykal RPG"), _sprites);

    gen.set_left_alignment();

    // New Game option
    bn::string_view new_game_prefix = (_cursor == 0) ? bn::string_view("> ") : bn::string_view("  ");
    bn::string<24> new_game_text;
    for (char c : new_game_prefix) new_game_text.push_back(c);
    for (char c : bn::string_view("New Game")) new_game_text.push_back(c);
    gen.generate(-60, 0, new_game_text, _sprites);

    if (_has_save)
    {
        bn::string_view cont_prefix = (_cursor == 1) ? bn::string_view("> ") : bn::string_view("  ");
        bn::string<24> cont_text;
        for (char c : cont_prefix) cont_text.push_back(c);
        for (char c : bn::string_view("Continue")) cont_text.push_back(c);
        gen.generate(-60, 20, cont_text, _sprites);
    }

    gen.set_center_alignment();
    gen.generate(0, 60, bn::string_view("Press A to select"), _sprites);
}
