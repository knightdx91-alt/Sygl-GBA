#include "gba.h"
#include "game.h"

int main(void) {
    irqInit();
    irqEnable(IRQ_VBLANK);

    game_init();

    while (1) {
        VBlankIntrWait();
        scanKeys();
        game_update();
        game_draw();
    }

    return 0;
}
