#include "bn_core.h"
#include "bn_keypad.h"
#include "bn_log.h"

int main()
{
    // Initialize Butano Core
    bn::core::init();
    
    bool at_start_menu = true;

    while(true)
    {
        if(at_start_menu)
        {
            // Log the start menu state
            BN_LOG("--- SYGL START MENU ---");
            BN_LOG("Press A to start");
            
            // Wait for user to press 'A'
            if(bn::keypad::a_pressed())
            {
                at_start_menu = false;
                BN_LOG("HELLO WORLD: Welcome to Sygl!");
            }
        }

        // Update the engine every frame
        bn::core::update();
    }
}
