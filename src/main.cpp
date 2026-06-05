#include "bn_core.h"
#include "bn_color.h"
#include "bn_bg_palettes.h"

int main()
{
    bn::core::init();
    // Dark navy — title screen placeholder until font/BG art is ready
    bn::bg_palettes::set_transparent_color(bn::color(2, 4, 8));
    while(true)
    {
        bn::core::update();
    }
}
