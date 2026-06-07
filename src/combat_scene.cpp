#include "combat_scene.h"
#include "bn_keypad.h"
#include "bn_bg_palettes.h"
#include "bn_color.h"
#include "bn_string.h"
#include "common_variable_8x16_sprite_font.h"
#include "bn_sprite_items_player.h"
#include "bn_sprite_items_enemy_bandit.h"
#include "bn_sprite_items_enemy_wolf.h"

// Simple deterministic "random" using a counter
static unsigned int g_rng = 12345u;
static int rng_next(int min, int max)
{
    g_rng = g_rng * 1664525u + 1013904223u;  // Numerical Recipes LCG
    int range = max - min + 1;
    if (range <= 0) return min;
    return min + (int)(g_rng >> 16) % range;
}

CombatScene::CombatScene(GameState& state, EnemyType enemy_type)
    : _state(state),
      _gen(common::variable_8x16_sprite_font),
      _enemy_type(enemy_type),
      _enemy_hp(enemy_stats(enemy_type).hp_max),
      _enemy_hp_max(enemy_stats(enemy_type).hp_max),
      _player_sprite(bn::sprite_items::player.create_sprite(-60, 20)),
      _enemy_sprite(enemy_type == EnemyType::BANDIT
          ? bn::sprite_items::enemy_bandit.create_sprite(60, -20)
          : bn::sprite_items::enemy_wolf.create_sprite(60, -20))
{
    bn::bg_palettes::set_transparent_color(bn::color(0, 0, 8));
    render_all();
}

CombatResult CombatScene::update()
{
    if (_delay > 0)
    {
        _delay--;
        return CombatResult::ONGOING;
    }

    if (_phase == Phase::GAME_OVER)
    {
        if (bn::keypad::a_pressed() || bn::keypad::start_pressed())
            return CombatResult::GAME_OVER;
        return CombatResult::ONGOING;
    }

    if (_phase == Phase::VICTORY)
    {
        if (bn::keypad::a_pressed() || bn::keypad::start_pressed())
            return CombatResult::VICTORY;
        return CombatResult::ONGOING;
    }

    if (_phase == Phase::PLAYER_TURN)
    {
        bool changed = false;
        if (bn::keypad::up_pressed())   { _cursor = 0; changed = true; }
        if (bn::keypad::down_pressed()) { _cursor = 1; changed = true; }
        if (changed) render_all();

        if (bn::keypad::a_pressed())
        {
            if (_cursor == 1)
            {
                // Flee: 50% chance
                if (rng_next(0, 1) == 0)
                {
                    render_status("Got away safely!");
                    _delay = 60;
                    _phase = Phase::VICTORY;
                    return CombatResult::ONGOING;
                }
                render_status("Can't escape!");
                _delay = 45;
                _phase = Phase::ENEMY_TURN;
                return CombatResult::ONGOING;
            }

            // Attack
            int dmg = roll_damage(_state.atk, enemy_stats(_enemy_type).def);
            _enemy_hp -= dmg;
            if (_enemy_hp <= 0) _enemy_hp = 0;
            render_all();
            render_status("You attack!");

            if (_enemy_hp <= 0)
            {
                int xp = enemy_stats(_enemy_type).xp_reward;
                _state.gain_xp(xp);
                render_all();
                render_status("Victory! XP gained.");
                _delay = 90;
                _phase = Phase::VICTORY;
            }
            else
            {
                _delay = 45;
                _phase = Phase::ENEMY_TURN;
            }
        }
    }
    else if (_phase == Phase::ENEMY_TURN)
    {
        // Enemy attacks
        int dmg = roll_damage(enemy_stats(_enemy_type).atk, _state.def);
        _state.hp -= dmg;
        if (_state.hp <= 0) _state.hp = 0;
        render_all();
        render_status("Enemy attacks!");

        if (_state.hp <= 0)
        {
            render_status("You were defeated...");
            _delay = 90;
            _phase = Phase::GAME_OVER;
        }
        else
        {
            _delay = 30;
            _phase = Phase::PLAYER_TURN;
        }
    }

    return CombatResult::ONGOING;
}

int CombatScene::roll_damage(int atk, int def)
{
    int base = atk - def;
    if (base < 1) base = 1;
    return base + rng_next(-1, 1);
}

void CombatScene::render_all()
{
    _sprites.clear();
    _gen.set_center_alignment();

    // Title bar
    const char* ename = enemy_stats(_enemy_type).name;
    bn::string<32> title;
    title.append(bn::string_view("VS "));
    for (int i = 0; ename[i]; ++i) title.push_back(ename[i]);
    _gen.generate(0, -72, title, _sprites);

    // Enemy HP
    _gen.set_right_alignment();
    bn::string<24> ehp;
    ehp.append(bn::string_view("HP "));
    int eh = _enemy_hp, ehm = _enemy_hp_max;
    if (eh >= 10) ehp.push_back('0' + eh / 10);
    ehp.push_back('0' + eh % 10);
    ehp.push_back('/');
    if (ehm >= 10) ehp.push_back('0' + ehm / 10);
    ehp.push_back('0' + ehm % 10);
    _gen.generate(116, -48, ehp, _sprites);

    // Player HP / MP
    _gen.set_left_alignment();
    bn::string<32> php;
    php.append(bn::string_view("HP "));
    int ph = _state.hp, phm = _state.hp_max;
    if (ph >= 100) php.push_back('0' + ph / 100);
    if (ph >= 10)  php.push_back('0' + (ph / 10) % 10);
    php.push_back('0' + ph % 10);
    php.push_back('/');
    if (phm >= 100) php.push_back('0' + phm / 100);
    if (phm >= 10)  php.push_back('0' + (phm / 10) % 10);
    php.push_back('0' + phm % 10);
    _gen.generate(-116, 48, php, _sprites);

    // Action menu
    _gen.set_left_alignment();
    bn::string_view atk_pfx = (_cursor == 0) ? "> " : "  ";
    bn::string_view fle_pfx = (_cursor == 1) ? "> " : "  ";
    bn::string<16> atk_line, fle_line;
    for (char c : atk_pfx) atk_line.push_back(c);
    atk_line.append(bn::string_view("Attack"));
    for (char c : fle_pfx) fle_line.push_back(c);
    fle_line.append(bn::string_view("Flee"));
    _gen.generate(-116, 56, atk_line, _sprites);
    _gen.generate(-116, 70, fle_line, _sprites);
}

void CombatScene::render_status(const char* msg)
{
    // Remove old status sprites (just re-render all with status at top)
    _sprites.clear();
    render_all();
    _gen.set_center_alignment();
    bn::string<48> s;
    for (int i = 0; msg[i]; ++i) s.push_back(msg[i]);
    _gen.generate(0, -56, s, _sprites);
}
