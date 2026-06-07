#include "bn_core.h"
#include "bn_color.h"
#include "bn_bg_palettes.h"
#include "bn_sprite_palettes.h"
#include "bn_keypad.h"
#include "bn_fixed.h"

// Phase 1 test ROM: atmospheric amber pulse on navy.
// Proves Butano pipeline + GitHub Pages deployment is live.
int main()
{
    bn::core::init();

    bn::bg_palettes::set_transparent_color(bn::color(2, 4, 8));
    bn::sprite_palettes::set_transparent_color(bn::color(2, 4, 8));

    // Fade target: warm amber matching the web UI gold (#f5c842 → GBA 5-bit ≈ 30,24,8)
    bn::bg_palettes::set_fade_color(bn::color(30, 24, 8));
    bn::sprite_palettes::set_fade_color(bn::color(30, 24, 8));

    int frame = 0;
    bool paused = false;

    while(true)
    {
        if (!paused)
            frame++;

        // Triangle wave 0..63..0 over 128 frames
        int t = frame & 0x7F;
        if (t >= 64) t = 127 - t;

        // Intensity range 0.0 → ~0.47 (keeps screen legible when art is added)
        bn::fixed intensity = bn::fixed(t) / 136;
        bn::bg_palettes::set_fade_intensity(intensity);
        bn::sprite_palettes::set_fade_intensity(intensity);

        // Start pauses the pulse (confirms input works in emulator)
        if (bn::keypad::start_pressed())
            paused = !paused;

        bn::core::update();
    }
}
