#include "gba.h"
#include "tiles.h"
#include <gba_video.h>
#include <gba_sprites.h>
#include <string.h>

/* Build a 4bpp tile row: 8 pixels packed into a u32.
   Each pixel is 4 bits; pixel 0 in bits [3:0]. */
#define PIX(p0,p1,p2,p3,p4,p5,p6,p7) \
    ((u32)(p0) | ((u32)(p1)<<4) | ((u32)(p2)<<8)  | ((u32)(p3)<<12) | \
     ((u32)(p4)<<16) | ((u32)(p5)<<20) | ((u32)(p6)<<24) | ((u32)(p7)<<28))

/* ---------- palettes ---------- */

static const u16 bg_pal[16] = {
    RGB15( 0, 0, 0),   /*  0 transparent/black  */
    RGB15( 9,16, 9),   /*  1 grass green        */
    RGB15( 4, 8, 4),   /*  2 dark green (tree)  */
    RGB15(17,17,17),   /*  3 light gray (stone) */
    RGB15( 8, 8, 8),   /*  4 dark gray (wall)   */
    RGB15( 4, 9,25),   /*  5 blue (water)       */
    RGB15( 2, 4,12),   /*  6 dark blue          */
    RGB15(16,10, 6),   /*  7 brown (door/trunk) */
    RGB15(24,18,12),   /*  8 tan (door center)  */
    RGB15(31,31,31),   /*  9 white              */
    RGB15(22,22,22),   /* 10 mortar gray        */
    RGB15(12,10, 8),   /* 11 dark floor         */
    RGB15(30,20,12),   /* 12 skin               */
    RGB15( 4,10,20),   /* 13 shirt blue         */
    RGB15(12, 8, 4),   /* 14 hair brown         */
    RGB15(31, 0, 0),   /* 15 red                */
};

static const u16 obj_pal[16] = {
    0,                 /*  0 transparent        */
    RGB15(30,20,12),   /*  1 skin               */
    RGB15(12, 8, 4),   /*  2 hair               */
    RGB15( 4,10,20),   /*  3 shirt              */
    RGB15( 8, 8,16),   /*  4 pants              */
    RGB15( 0, 0, 0),   /*  5 outline black      */
    RGB15(31,24, 0),   /*  6 gold               */
    RGB15(31,31,31),   /*  7 white              */
    0,0,0,0,0,0,0,0,   /* 8-15 unused           */
};

/* ---------- BG tile data: 6 tiles × 8 rows ---------- */

/* Each tile is 8 rows of u32 (4bpp, 32 bytes per tile). */
static const u32 tile_data[6][8] = {
    /* 0 — Grass */
    {
        PIX(1,1,1,1,1,1,1,1),
        PIX(1,1,2,1,1,1,1,1),
        PIX(1,1,1,1,1,1,1,1),
        PIX(1,1,1,1,1,2,1,1),
        PIX(1,1,1,1,1,1,1,1),
        PIX(2,1,1,1,1,1,1,1),
        PIX(1,1,1,1,1,1,1,1),
        PIX(1,1,1,2,1,1,1,1),
    },
    /* 1 — Stone/Path */
    {
        PIX(3,0xA,3,0xA,3,0xA,3,0xA),
        PIX(0xA,3,0xA,3,0xA,3,0xA,3),
        PIX(3,0xA,3,0xA,3,0xA,3,0xA),
        PIX(0xA,3,0xA,3,0xA,3,0xA,3),
        PIX(3,0xA,3,0xA,3,0xA,3,0xA),
        PIX(0xA,3,0xA,3,0xA,3,0xA,3),
        PIX(3,0xA,3,0xA,3,0xA,3,0xA),
        PIX(0xA,3,0xA,3,0xA,3,0xA,3),
    },
    /* 2 — Water */
    {
        PIX(5,5,6,5,5,6,5,5),
        PIX(6,5,5,5,6,5,5,5),
        PIX(5,5,5,6,5,5,5,6),
        PIX(5,6,5,5,5,6,5,5),
        PIX(5,5,6,5,5,6,5,5),
        PIX(6,5,5,5,6,5,5,5),
        PIX(5,5,5,6,5,5,5,6),
        PIX(5,6,5,5,5,6,5,5),
    },
    /* 3 — Wall */
    {
        PIX(4,4,4,4,4,4,4,4),
        PIX(0xA,4,4,4,4,4,4,0xA),
        PIX(4,4,4,4,4,4,4,4),
        PIX(4,4,0xA,4,4,0xA,4,4),
        PIX(4,4,4,4,4,4,4,4),
        PIX(0xA,4,4,4,4,4,4,0xA),
        PIX(4,4,4,4,4,4,4,4),
        PIX(4,4,0xA,4,4,0xA,4,4),
    },
    /* 4 — Door */
    {
        PIX(7,7,7,7,7,7,7,7),
        PIX(7,8,8,8,8,8,8,7),
        PIX(7,8,8,8,8,8,8,7),
        PIX(7,8,8,8,8,8,8,7),
        PIX(7,8,8,8,8,8,8,7),
        PIX(7,8,8,8,8,8,8,7),
        PIX(7,8,8,8,8,8,8,7),
        PIX(7,7,7,7,7,7,7,7),
    },
    /* 5 — Tree */
    {
        PIX(0,0,2,2,2,2,0,0),
        PIX(0,2,2,2,2,2,2,0),
        PIX(2,2,2,2,2,2,2,2),
        PIX(2,2,2,2,2,2,2,2),
        PIX(0,0,0,7,7,0,0,0),
        PIX(0,0,0,7,7,0,0,0),
        PIX(0,0,0,7,7,0,0,0),
        PIX(0,0,0,7,7,0,0,0),
    },
};

/* ---------- OBJ tile: player sprite (8×8, facing down) ---------- */
/* OBJ palette indices: 0=transparent 1=skin 2=hair 3=shirt 4=pants 5=outline */

static const u32 player_tile[8] = {
    PIX(0,0,5,2,2,5,0,0),
    PIX(0,5,2,2,2,2,5,0),
    PIX(5,1,1,5,5,1,1,5),
    PIX(5,1,1,1,1,1,1,5),
    PIX(0,5,3,3,3,3,5,0),
    PIX(0,5,3,3,3,3,5,0),
    PIX(0,5,4,5,5,4,5,0),
    PIX(0,5,4,5,5,4,5,0),
};

/* ---------- public API ---------- */

void tiles_init(void) {
    int i;

    /* BG palette: sub-palette 0 (indices 0-15) */
    for (i = 0; i < 16; i++)
        BG_PALETTE[i] = bg_pal[i];

    /* OBJ palette: sub-palette 0 */
    for (i = 0; i < 16; i++)
        OBJ_PALETTE[i] = obj_pal[i];

    /* BG charblock 0: tiles 0-5 */
    {
        u32 *dest = (u32 *)tile_mem[0];
        int t;
        for (t = 0; t < 6; t++) {
            int r;
            for (r = 0; r < 8; r++)
                dest[t * 8 + r] = tile_data[t][r];
        }
    }

    /* OBJ charblock (tile_mem[4]): sprite tile 0 */
    {
        u32 *dest = (u32 *)tile_mem[4];
        int r;
        for (r = 0; r < 8; r++)
            dest[r] = player_tile[r];
    }
}

void oam_clear(void) {
    int i;
    for (i = 0; i < 128; i++) {
        OAM[i].attr0 = 0;
        OAM[i].attr1 = 0;
        OAM[i].attr2 = 0;
    }
}
