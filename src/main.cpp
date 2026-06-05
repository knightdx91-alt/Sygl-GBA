#include "bn_core.h"
#include "bn_bg_palettes.h"

int main()
{
    bn::core::init();
    bn::bg_palettes::set_transparent_color(bn::color(2, 4, 8));
    while(true)
    {
        bn::core::update();
    }
}
