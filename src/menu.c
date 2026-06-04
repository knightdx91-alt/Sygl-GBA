#include "gba.h"
#include "menu.h"
#include "player.h"
#include <stdio.h>

static const char *SYGL_NAMES[5] = {
    "Duality",
    "Waves",
    "Terra",
    "Tempus",
    "Blood",
};

static const char *SYGL_DESC[5] = {
    "Aren. Day & night. Heal and harm.",
    "Kenmei. The tide. Adapt, endure.",
    "Naisura. The earth. High DEF.",
    "Raizen. The storm. High SPD.",
    "Amelia. The price. High ATK.",
};

static int cursor    = 0;
static int confirmed = 0;

/* ------------------------------------------------------------------ */
/* menu_init_console                                                    */
/* Call once when entering any console-mode state (title / sygl select  */
/* / pause). Sets up the text console on BG0.                          */
/* ------------------------------------------------------------------ */
void menu_init_console(void) {
    consoleDemoInit();
}

/* ------------------------------------------------------------------ */
/* Sygl selection                                                       */
/* ------------------------------------------------------------------ */

void menu_update_sygl_select(void) {
    u32 keys = keysDown();
    if ((keys & KEY_DOWN) && cursor < 4) cursor++;
    if ((keys & KEY_UP)   && cursor > 0) cursor--;
    if (keys & KEY_A) {
        player_set_sygl((SyglType)cursor);
        confirmed = 1;
    }
}

int menu_sygl_confirmed(void) {
    int c = confirmed;
    confirmed = 0;
    return c;
}

void menu_draw_sygl_select(void) {
    int i;
    /* Clear screen each frame so highlight updates. */
    iprintf("\x1b[2J"); /* clear console */

    /* Title */
    iprintf("\x1b[1;8H== Choose Your Sygl ==");

    /* List the five sygls */
    for (i = 0; i < 5; i++) {
        if (i == cursor)
            iprintf("\x1b[%d;4H> %-6s  %s", 4 + i * 2, SYGL_NAMES[i], SYGL_DESC[i]);
        else
            iprintf("\x1b[%d;4H  %-6s  %s", 4 + i * 2, SYGL_NAMES[i], SYGL_DESC[i]);
    }

    iprintf("\x1b[16;4HA: Confirm   UP/DOWN: Move");
}

/* ------------------------------------------------------------------ */
/* Pause menu                                                           */
/* ------------------------------------------------------------------ */

static const char *PAUSE_OPTIONS[] = {
    "Items",
    "Magic",
    "Status",
    "Save",
    "Resume",
};
#define PAUSE_COUNT 5

static int pause_cursor = 0;

void menu_update(void) {
    u32 keys = keysDown();
    if ((keys & KEY_DOWN) && pause_cursor < PAUSE_COUNT - 1) pause_cursor++;
    if ((keys & KEY_UP)   && pause_cursor > 0)               pause_cursor--;
    /* TODO: handle A confirm for each option */
}

void menu_draw(void) {
    int i;
    iprintf("\x1b[2J");
    iprintf("\x1b[1;10H== Menu ==");
    for (i = 0; i < PAUSE_COUNT; i++) {
        if (i == pause_cursor)
            iprintf("\x1b[%d;8H> %s", 4 + i * 2, PAUSE_OPTIONS[i]);
        else
            iprintf("\x1b[%d;8H  %s", 4 + i * 2, PAUSE_OPTIONS[i]);
    }
    iprintf("\x1b[16;4HB: Close menu");
}
