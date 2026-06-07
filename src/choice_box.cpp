#include "choice_box.h"
#include "bn_keypad.h"

static constexpr int PROMPT_Y  = 36;
static constexpr int OPTION_AY = 52;
static constexpr int OPTION_BY = 68;

void ChoiceBox::open(bn::sprite_text_generator& gen,
                     bn::string_view prompt,
                     bn::string_view option_a,
                     bn::string_view option_b)
{
    _prompt   = prompt;
    _option_a = option_a;
    _option_b = option_b;
    _cursor   = 0;
    _open     = true;
    render(gen);
}

int ChoiceBox::update(bn::sprite_text_generator& gen)
{
    if (!_open) return -1;

    bool changed = false;
    if (bn::keypad::down_pressed()) { _cursor = 1; changed = true; }
    if (bn::keypad::up_pressed())   { _cursor = 0; changed = true; }
    if (changed) render(gen);

    if (bn::keypad::a_pressed())
    {
        _open = false;
        _sprites.clear();
        return _cursor;
    }
    return -1;
}

void ChoiceBox::render(bn::sprite_text_generator& gen)
{
    _sprites.clear();
    gen.set_center_alignment();
    gen.generate(0, PROMPT_Y, _prompt, _sprites);

    gen.set_left_alignment();

    // Option A
    bn::string_view prefix_a = (_cursor == 0) ? "> " : "  ";
    bn::string<40> line_a;
    for (char c : prefix_a) line_a.push_back(c);
    for (char c : _option_a) line_a.push_back(c);
    gen.generate(-80, OPTION_AY, line_a, _sprites);

    // Option B
    bn::string_view prefix_b = (_cursor == 1) ? "> " : "  ";
    bn::string<40> line_b;
    for (char c : prefix_b) line_b.push_back(c);
    for (char c : _option_b) line_b.push_back(c);
    gen.generate(-80, OPTION_BY, line_b, _sprites);
}
