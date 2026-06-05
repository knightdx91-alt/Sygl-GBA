#include "bn_core.h"
#include "bn_color.h"
#include "bn_bg_palettes.h"
#include "bn_text.h"
#include "bn_sprite_text_generator.h"
#include "bn_sprites.h"

int main()
{
    bn::core::init();
    
    // Set transparent color for backgrounds
    bn::bg_palettes::set_transparent_color(bn::color(2, 4, 8));
    
    // Create a white color for text
    bn::color text_color(31, 31, 31); // White
    
    // Create sprite text generator for displaying text
    bn::sprite_text_generator text_gen(bn::sprite_font());
    text_gen.set_center(true);
    
    // Display "The Heir's School" at the center of the screen
    auto text_sprites = text_gen.generate(120, 40, "The Heir's School");
    
    // Add the sprites to the screen
    for (auto& sprite : text_sprites) {
        bn::sprites::add(sprite);
    }
    
    // Test the core loop and rendering
    int frame_count = 0;
    
    while(true)
    {
        frame_count++;
        bn::core::update();
    }
    
    return 0;
}
