#include "bn_core.h"
#include "bn_display.h"
#include "bn_bg_palettes.h"
#include "bn_sprite_text_generator.h"
#include "bn_sprite_ptr.h"
#include "bn_vector.h"
#include "variable_8x16_sprite_font.h"

int main()
{
    bn::core::init();

    // Dark blue background
    bn::bg_palettes::set_transparent_color(bn::color(2, 4, 8));

    bn::sprite_text_generator text_gen(variable_8x16_sprite_font);
    text_gen.set_center_alignment();

    bn::vector<bn::sprite_ptr, 32> sprites;
    text_gen.generate(0, -20, "SYGL", sprites);
    text_gen.generate(0,   0, "A Magykal RPG", sprites);
    text_gen.generate(0,  20, "Press START", sprites);

    while(true)
    {
        bn::core::update();
    }
}
