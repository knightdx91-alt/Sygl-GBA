#include "bn_core.h"
#include "bn_color.h"
#include "bn_bg_palettes.h"
#include "bn_keypad.h"
#include "bn_fixed.h"

// Phase 1 ROM: ambient amber pulse on navy. Pipeline validation build.
// Text rendering added in Phase 2 once font asset path is confirmed.
int main()
{
    bn::core::init();

    bn::bg_palettes::set_transparent_color(bn::color(2, 4, 8));
    bn::bg_palettes::set_fade_color(bn::color(30, 24, 8));

    int frame = 0;
    while(true)
    {
        frame++;

        // Triangle wave 0..63..0 over 128 frames → gentle amber pulse
        int t = frame & 0x7F;
        if(t >= 64) t = 127 - t;
        bn::bg_palettes::set_fade_intensity(bn::fixed(t) / 136);

        bn::core::update();
    }
}
