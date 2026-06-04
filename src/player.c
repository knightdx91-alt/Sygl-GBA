#include "gba.h"
#include "player.h"
#include "map.h"

Player player;

static const u16 SYGL_BASE[5][5] = {
    { 40, 30, 8, 4, 7 },
    { 45, 40, 6, 6, 6 },
    { 55, 25, 7, 9, 4 },
    { 38, 35, 6, 4, 9 },
    { 35, 50, 5, 3, 7 },
};

void player_init(void) {
    player.map_x = 5;
    player.map_y = 5;
    player.px = player.map_x * TILE_SIZE;
    player.py = player.map_y * TILE_SIZE;
    player.dir = 0;
    player.move_timer = 0;
    player.sygl = SYGL_NONE;
    player.level = 1;
    player.xp = 0;
}

void player_set_sygl(SyglType sygl) {
    player.sygl = sygl;
    player.hp_max = SYGL_BASE[sygl][0];
    player.mp_max = SYGL_BASE[sygl][1];
    player.atk    = SYGL_BASE[sygl][2];
    player.def    = SYGL_BASE[sygl][3];
    player.spd    = SYGL_BASE[sygl][4];
    player.hp     = player.hp_max;
    player.mp     = player.mp_max;
}

void player_update(void) {
    if (player.move_timer > 0) { player.move_timer--; return; }

    u32 keys = keysHeld();
    s16 nx = player.map_x;
    s16 ny = player.map_y;

    if (keys & KEY_DOWN)  { ny++; player.dir = 0; }
    if (keys & KEY_UP)    { ny--; player.dir = 1; }
    if (keys & KEY_LEFT)  { nx--; player.dir = 2; }
    if (keys & KEY_RIGHT) { nx++; player.dir = 3; }

    if (nx != player.map_x || ny != player.map_y) {
        if (!map_is_solid(nx, ny)) {
            player.map_x = nx;
            player.map_y = ny;
            player.px = nx * TILE_SIZE;
            player.py = ny * TILE_SIZE;
            player.move_timer = 8;
        }
    }
}

void player_draw(void) {
    /* Player is always centered on screen; map scrolls around them. */
    int sx = SCREEN_W / 2 - TILE_SIZE / 2;  /* 116 */
    int sy = SCREEN_H / 2 - TILE_SIZE / 2;  /*  76 */

    /*
     * attr0: bits[7:0]  = Y position
     *        bits[13]   = 0 for 4bpp (16-color)
     *        bits[15:14]= 00 for normal (square shape)
     * attr1: bits[8:0]  = X position
     *        bits[15:14]= 00 → 8×8 for square shape
     * attr2: bits[9:0]  = tile number (0), bits[13:12] = palette number (0)
     */
    OAM[0].attr0 = (u16)((sy & 0xFF) | (0 << 13) | (0 << 14));
    OAM[0].attr1 = (u16)((sx & 0x1FF) | (0 << 14));
    OAM[0].attr2 = 0; /* tile 0, palette 0 */
}
