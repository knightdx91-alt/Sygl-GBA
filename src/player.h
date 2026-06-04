#pragma once
#include <gba_types.h>

typedef enum {
    SYGL_NONE    = -1,
    SYGL_DUALITY = 0,  /* Aren       — balance, heal/harm  */
    SYGL_WAVES   = 1,  /* Kenmei     — water, endurance    */
    SYGL_TERRA   = 2,  /* Naisura    — earth, unmoving     */
    SYGL_TEMPUS  = 3,  /* Raizen     — storm, speed        */
    SYGL_BLOOD   = 4,  /* Amelia     — drain, sacrifice    */
} SyglType;

typedef struct {
    s16 map_x, map_y;
    s16 px, py;
    u8  dir;
    u8  move_timer;
    SyglType sygl;
    u16 hp, hp_max;
    u16 mp, mp_max;
    u16 atk, def, spd;
    u16 level, xp;
} Player;

extern Player player;

void player_init(void);
void player_update(void);
void player_draw(void);
void player_set_sygl(SyglType sygl);
