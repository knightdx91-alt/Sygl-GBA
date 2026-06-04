#pragma once
#include <stdio.h>
#include <gba_console.h>
#include <gba_video.h>
#include <gba_interrupt.h>
#include <gba_systemcalls.h>
#include <gba_input.h>
#include <gba_sprites.h>

#define SCREEN_W 240
#define SCREEN_H 160
#define TILE_SIZE      8
#define MAP_W          32
#define MAP_H          32

#define RGB15(r,g,b)  (((b)<<10)|((g)<<5)|(r))
#define BLACK   RGB15( 0, 0, 0)
#define WHITE   RGB15(31,31,31)
#define RED     RGB15(31, 0, 0)
#define GREEN   RGB15( 0,31, 0)
#define BLUE    RGB15( 0, 0,31)
#define GOLD    RGB15(31,24, 0)
#define PURPLE  RGB15(20, 0,31)
