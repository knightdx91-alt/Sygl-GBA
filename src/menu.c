#include "gba.h"
#include "menu.h"
#include "player.h"

static const char *SYGL_NAMES[5]  = { "Fire", "Water", "Earth", "Air", "Void" };
static const char *SYGL_DESC[5] = {
    "Passion and destruction. High attack.",
    "Balance and flow. High MP.",
    "Endurance and strength. High defense.",
    "Speed and freedom. High agility.",
    "Mystery and power. High magic.",
};

static int cursor = 0;
static int confirmed = 0;

void menu_update_sygl_select(void) {
    u32 keys = keysDown();
    if (keys & KEY_DOWN && cursor < 4) cursor++;
    if (keys & KEY_UP   && cursor > 0) cursor--;
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
    (void)SYGL_NAMES;
    (void)SYGL_DESC;
    (void)cursor;
    // TODO: render sygl selection with tiles/text
}

void menu_update(void) {
    // TODO: pause menu (Items / Magic / Status / Save)
}

void menu_draw(void) {
    // TODO: draw pause menu panel
}
