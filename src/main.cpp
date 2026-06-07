#include "bn_core.h"
#include "bn_color.h"
#include "bn_bg_palettes.h"
#include "bn_sprite_palettes.h"
#include "bn_keypad.h"
#include "bn_fixed.h"
#include "bn_sprite_text_generator.h"
#include "bn_vector.h"
#include "common_fixed_8x8_font.h"

// Phase 1 title screen: text rendered via Butano sprite text generator.
int main()
{
    bn::core::init();

    // Deep navy backdrop
    bn::bg_palettes::set_transparent_color(bn::color(2, 4, 8));

    // Set up text generator using the bundled fixed 8x8 font
    bn::sprite_text_generator text_gen(common::fixed_8x8_font);
    text_gen.set_center_alignment();

    bn::vector<bn::sprite_ptr, 8>  title_sprites;   // "SYGL"
    bn::vector<bn::sprite_ptr, 16> sub_sprites;     // "A MAGYKAL RPG"
    bn::vector<bn::sprite_ptr, 16> start_sprites;   // "PRESS START"

    text_gen.generate(0, -40, "SYGL",         title_sprites);
    text_gen.generate(0, -10, "A MAGYKAL RPG", sub_sprites);
    text_gen.generate(0,  40, "PRESS START",   start_sprites);

    // Amber fade on background matches web UI gold
    bn::bg_palettes::set_fade_color(bn::color(30, 24, 8));

    int frame = 0;
    while(true)
    {
        frame++;

        // "PRESS START" blinks every 30 frames
        bool blink = (frame / 30) % 2 == 0;
        for(auto& s : start_sprites) s.set_visible(blink);

        // Gentle ambient pulse on the background
        int t = frame & 0x7F;
        if(t >= 64) t = 127 - t;
        bn::bg_palettes::set_fade_intensity(bn::fixed(t) / 136);

        bn::core::update();
    }
}
