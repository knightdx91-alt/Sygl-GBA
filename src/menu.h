#pragma once
#include "player.h"

void menu_init_console(void);      /* call on state entry for console-mode states */

void menu_update_sygl_select(void);
int  menu_sygl_confirmed(void);
void menu_draw_sygl_select(void);

void menu_update(void);
void menu_draw(void);
