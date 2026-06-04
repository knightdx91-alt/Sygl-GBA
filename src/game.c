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

static GameState state      = STATE_TITLE;
static GameState prev_state = STATE_BATTLE; /* force init on first frame */
static int       frame      = 0;
static int       draw_dirty = 1; /* 1 = need to redraw console screen */

/* ------------------------------------------------------------------ */
/* Enter a new state: configure hardware for that state                */
/* ------------------------------------------------------------------ */
static void enter_state(GameState s) {
    prev_state = state;
    state      = s;
    draw_dirty = 1;

    switch (s) {
    case STATE_OVERWORLD:
        SetMode(MODE_0 | BG0_ON | OBJ_ENABLE | OBJ_1D_MAP);
        map_init();
        break;

    case STATE_TITLE:
    case STATE_SYGL_SELECT:
    case STATE_MENU:
        /* Console text mode: uses BG0 in a special tiled-text config */
        menu_init_console();
        break;

    default:
        break;
    }
}

/* ------------------------------------------------------------------ */

void game_init(void) {
    frame = 0;
    player_init();
    enter_state(STATE_TITLE);
}

void game_update(void) {
    frame++;
    u32 keys = keysDown();

    switch (state) {
    case STATE_TITLE:
        if (keys & KEY_START) enter_state(STATE_SYGL_SELECT);
        break;

    case STATE_SYGL_SELECT:
        menu_update_sygl_select();
        if (menu_sygl_confirmed()) enter_state(STATE_OVERWORLD);
        break;

    case STATE_OVERWORLD:
        player_update();
        if (keys & KEY_START) enter_state(STATE_MENU);
        break;

    case STATE_MENU:
        menu_update();
        if (keys & KEY_B) enter_state(STATE_OVERWORLD);
        break;

    default:
        break;
    }
}

void game_draw(void) {
    switch (state) {
    case STATE_TITLE:
        /* Draw once per entry (draw_dirty), then only on START press
           handled in update. We redraw every frame so ANSI stays clean. */
        iprintf("\x1b[2J");
        iprintf("\x1b[6;9H  S  Y  G  L");
        iprintf("\x1b[8;7H  A Magykal RPG");
        iprintf("\x1b[12;5HPress START to begin");
        iprintf("\x1b[17;4H  (c) 2024 The Author");
        break;

    case STATE_SYGL_SELECT:
        menu_draw_sygl_select();
        break;

    case STATE_OVERWORLD:
        map_draw();
        player_draw();
        break;

    case STATE_MENU:
        menu_draw();
        break;

    default:
        break;
    }
}
