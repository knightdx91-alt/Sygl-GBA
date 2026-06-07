#pragma once
#include "bn_vector.h"
#include "bn_sprite_ptr.h"
#include "bn_sprite_text_generator.h"
#include "bn_string_view.h"

// Two-option choice prompt. Shown after TextBox closes.
class ChoiceBox
{
public:
    ChoiceBox() = default;

    void open(bn::sprite_text_generator& gen,
              bn::string_view prompt,
              bn::string_view option_a,
              bn::string_view option_b);

    // Returns -1 while open, 0 if A chosen, 1 if B chosen.
    int update(bn::sprite_text_generator& gen);

    bool is_open() const { return _open; }

private:
    void render(bn::sprite_text_generator& gen);

    bn::vector<bn::sprite_ptr, 64> _sprites;
    bn::string_view _prompt;
    bn::string_view _option_a;
    bn::string_view _option_b;
    int _cursor = 0;
    bool _open = false;
};
