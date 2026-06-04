#include "gba.h"
#include "game.h"
#include "player.h"
#include "map.h"
#include "menu.h"

typedef enum {
    STATE_TITLE,
    STATE_SYGL_SELECT,
    STATE_OVERWORLD,
    STATE_MENU,
    STATE_BATTLE,
    STATE_DIALOGUE,
} GameState;

static GameState state = STATE_TITLE;
static int frame = 0;

void game_init(void) {
    SetMode(MODE_0 | OBJ_ENABLE | OBJ_1D_MAP);
    map_init();
    player_init();
    state = STATE_TITLE;
    frame = 0;
}

void game_update(void) {
    frame++;
    u32 keys = keysDown();

    switch (state) {
    case STATE_TITLE:
        if (keys & KEY_START) state = STATE_SYGL_SELECT;
        break;
    case STATE_SYGL_SELECT:
        menu_update_sygl_select();
        if (menu_sygl_confirmed()) state = STATE_OVERWORLD;
        break;
    case STATE_OVERWORLD:
        player_update();
        if (keys & KEY_START) state = STATE_MENU;
        break;
    case STATE_MENU:
        menu_update();
        if (keys & KEY_B) state = STATE_OVERWORLD;
        break;
    default:
        break;
    }
}

void game_draw(void) {
    switch (state) {
    case STATE_TITLE:          break;
    case STATE_SYGL_SELECT:    menu_draw_sygl_select(); break;
    case STATE_OVERWORLD:      map_draw(); player_draw(); break;
    case STATE_MENU:           menu_draw(); break;
    default:                   break;
    }
}
