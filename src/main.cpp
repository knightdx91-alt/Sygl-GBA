#include "bn_core.h"
#include "bn_sprite_text_generator.h"
#include "bn_vector.h"
#include "bn_sprite_ptr.h"

// Include the official font header we just copied over
#include "common_variable_8x16_sprite_font.h"

int main()
{
    // Initialize the engine
    bn::core::init();
    
    // Set up the text generator using Butano's pre-built font data
    bn::sprite_text_generator text_generator(common::variable_8x16_sprite_font);
    
    // Tell it to center our text
    text_generator.set_center_alignment();
    
    // The GBA needs a vector array to store the generated letter sprites
    bn::vector<bn::sprite_ptr, 32> text_sprites;
    
    // Draw "HELLO WORLD!" at X=0, Y=0 (dead center of the screen)
    text_generator.generate(0, 0, "HELLO WORLD!", text_sprites);

    while(true)
    {
        bn::core::update();
    }
}
