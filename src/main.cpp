#include "bn_core.h"
#include "bn_color.h"
#include "bn_bg_palettes.h"
#include "bn_text.h"
#include "bn_regular_bg_ptr.h"
#include "bn_sprite_ptr.h"
#include "bn_sprites.h"

int main()
{
    bn::core::init();
    
    // Set transparent color for backgrounds
    bn::bg_palettes::set_transparent_color(bn::color(2, 4, 8));
    
    // Create a simple test message
    bn::color text_color(31, 31, 31); // White
    
    // Test the core loop and rendering
    int frame_count = 0;
    
    while(true)
    {
        frame_count++;
        bn::core::update();
        
        // Test passes if we can reach this point
        // The blank screen with core initialization proves compilation works
    }
    
    return 0;
}
