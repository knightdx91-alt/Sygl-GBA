#include "gba.h"
#include "battle.h"
#include "player.h"

typedef enum {
    PHASE_PLAYER_TURN,
    PHASE_ENEMY_TURN,
    PHASE_VICTORY,
    PHASE_DEFEAT,
} BattlePhase;

static Enemy cur_enemy;
static BattlePhase phase;
static int over;

static int calc_damage(u16 atk, u16 def) {
    int dmg = (int)atk * 2 - (int)def;
    return dmg < 1 ? 1 : dmg;
}

void battle_start(const Enemy *enemy) {
    cur_enemy = *enemy;
    phase = PHASE_PLAYER_TURN;
    over = 0;
}

void battle_update(void) {
    if (over) return;
    u32 keys = keysDown();

    switch (phase) {
    case PHASE_PLAYER_TURN:
        if (keys & KEY_A) {
            int dmg = calc_damage(player.atk, cur_enemy.def);
            if (dmg > cur_enemy.hp) dmg = cur_enemy.hp;
            cur_enemy.hp -= dmg;
            phase = cur_enemy.hp == 0 ? PHASE_VICTORY : PHASE_ENEMY_TURN;
        }
        break;
    case PHASE_ENEMY_TURN:
        {
            int dmg = calc_damage(cur_enemy.atk, player.def);
            if (dmg > player.hp) dmg = player.hp;
            player.hp -= dmg;
            phase = player.hp == 0 ? PHASE_DEFEAT : PHASE_PLAYER_TURN;
        }
        break;
    case PHASE_VICTORY:
        if (keys & (KEY_A | KEY_START)) { player.xp += cur_enemy.xp_reward; over = 1; }
        break;
    case PHASE_DEFEAT:
        if (keys & (KEY_A | KEY_START)) over = 1;
        break;
    }
}

void battle_draw(void) {
    // TODO: enemy sprite, HP bars, command window
}

int battle_is_over(void) { return over; }
