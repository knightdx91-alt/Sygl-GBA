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

int CombatScene::roll_damage(int atk, int def)
{
    int base = atk - def;
    if (base < 1) base = 1;
    return base + rng_next(-1, 1);
}

int CombatScene::roll_magic(int mag, int power)
{
    int base = power + mag / 2;
    return base + rng_next(-2, 2);
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
        Character& pc = _state.party.player();

        // Tick status effects before player acts
        tick_status_effects();
        if (_phase != Phase::PLAYER_TURN) return CombatResult::ONGOING;
        if (pc.hp <= 0)
        {
            render_status("You were defeated...");
            _delay = 90;
            _phase = Phase::GAME_OVER;
            return CombatResult::ONGOING;
        }

        bool changed = false;
        if (bn::keypad::up_pressed())    { _cursor = (_cursor + 3) % 4; changed = true; }
        if (bn::keypad::down_pressed())  { _cursor = (_cursor + 1) % 4; changed = true; }
        if (bn::keypad::left_pressed())  { _cursor = (_cursor + 3) % 4; changed = true; }
        if (bn::keypad::right_pressed()) { _cursor = (_cursor + 1) % 4; changed = true; }
        if (changed) render_all();

        if (bn::keypad::a_pressed())
        {
            if (_cursor == 0)
            {
                // Attack
                int dmg = roll_damage(pc.eff_atk(), enemy_stats(_enemy_type).def);
                _enemy_hp -= dmg;
                if (_enemy_hp < 0) _enemy_hp = 0;
                render_all();
                render_status("You attack!");

                if (_enemy_hp <= 0)
                {
                    int xp = enemy_stats(_enemy_type).xp_reward;
                    pc.gain_xp(xp);
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
            else if (_cursor == 1)
            {
                // Magic
                if (pc.has_status(StatusFlag::SILENCE))
                {
                    render_status("Silenced!");
                    return CombatResult::ONGOING;
                }
                if (pc.spell_count == 0)
                {
                    render_status("No spells!");
                    return CombatResult::ONGOING;
                }
                _phase = Phase::MAGIC_SELECT;
                _sub_cursor = 0;
                render_all();
            }
            else if (_cursor == 2)
            {
                // Item - check if any combat_usable items exist
                bool has_usable = false;
                for (int i = 0; i < _state.party.inventory.count; ++i)
                {
                    ItemId iid = _state.party.inventory.slots[i].id;
                    if (iid != ItemId::NONE && item_def(iid).combat_usable)
                    {
                        has_usable = true;
                        break;
                    }
                }
                if (!has_usable)
                {
                    render_status("No items!");
                    return CombatResult::ONGOING;
                }
                _phase = Phase::ITEM_SELECT;
                _sub_cursor = 0;
                render_all();
            }
            else if (_cursor == 3)
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
        }
    }
    else if (_phase == Phase::MAGIC_SELECT)
    {
        Character& pc = _state.party.player();
        int list_size = pc.spell_count + 1; // +1 for Back

        bool changed = false;
        if (bn::keypad::up_pressed())   { _sub_cursor = (_sub_cursor + list_size - 1) % list_size; changed = true; }
        if (bn::keypad::down_pressed()) { _sub_cursor = (_sub_cursor + 1) % list_size; changed = true; }
        if (changed) render_all();

        if (bn::keypad::b_pressed())
        {
            _phase = Phase::PLAYER_TURN;
            render_all();
            return CombatResult::ONGOING;
        }

        if (bn::keypad::a_pressed())
        {
            if (_sub_cursor == pc.spell_count)
            {
                // Back
                _phase = Phase::PLAYER_TURN;
                render_all();
                return CombatResult::ONGOING;
            }

            SpellId sid = pc.known_spells[_sub_cursor];
            const SpellDef& spell = spell_def(sid);

            if (pc.mp < spell.mp_cost)
            {
                render_status("Not enough MP!");
                return CombatResult::ONGOING;
            }

            pc.mp -= spell.mp_cost;

            if (spell.is_heal)
            {
                if (sid == SpellId::CLEANSE)
                {
                    pc.clear_all_status();
                    render_all();
                    render_status("Cleansed status!");
                }
                else
                {
                    int healed = spell.power;
                    pc.hp += healed;
                    int hmax = pc.eff_hp_max();
                    if (pc.hp > hmax) pc.hp = hmax;
                    render_all();
                    render_status("HP restored!");
                }
            }
            else if (sid == SpellId::DRAIN)
            {
                int dmg = roll_magic(pc.eff_mag(), spell.power);
                _enemy_hp -= dmg;
                if (_enemy_hp < 0) _enemy_hp = 0;
                int heal = dmg / 2;
                pc.hp += heal;
                int hmax = pc.eff_hp_max();
                if (pc.hp > hmax) pc.hp = hmax;
                render_all();
                render_status("Drained HP!");
            }
            else
            {
                int dmg = roll_magic(pc.eff_mag(), spell.power);
                _enemy_hp -= dmg;
                if (_enemy_hp < 0) _enemy_hp = 0;
                if (spell.inflicts != 0)
                    _enemy_status |= spell.inflicts;
                render_all();
                render_status("Spell cast!");
            }

            if (_enemy_hp <= 0)
            {
                int xp = enemy_stats(_enemy_type).xp_reward;
                pc.gain_xp(xp);
                render_all();
                render_status("Victory! XP gained.");
                _delay = 90;
                _phase = Phase::VICTORY;
                return CombatResult::ONGOING;
            }

            _phase = Phase::ENEMY_TURN;
            _delay = 45;
        }
    }
    else if (_phase == Phase::ITEM_SELECT)
    {
        // Build combat-usable item list (max 8)
        ItemId usable_ids[8] = {};
        int usable_count = 0;
        for (int i = 0; i < _state.party.inventory.count && usable_count < 8; ++i)
        {
            ItemId iid = _state.party.inventory.slots[i].id;
            if (iid != ItemId::NONE && item_def(iid).combat_usable)
                usable_ids[usable_count++] = iid;
        }

        int list_size = usable_count + 1; // +1 for Back

        bool changed = false;
        if (bn::keypad::up_pressed())   { _sub_cursor = (_sub_cursor + list_size - 1) % list_size; changed = true; }
        if (bn::keypad::down_pressed()) { _sub_cursor = (_sub_cursor + 1) % list_size; changed = true; }
        if (changed) render_all();

        if (bn::keypad::b_pressed())
        {
            _phase = Phase::PLAYER_TURN;
            render_all();
            return CombatResult::ONGOING;
        }

        if (bn::keypad::a_pressed())
        {
            if (_sub_cursor == usable_count)
            {
                // Back
                _phase = Phase::PLAYER_TURN;
                render_all();
                return CombatResult::ONGOING;
            }

            ItemId used_id = usable_ids[_sub_cursor];
            const ItemDef& idef = item_def(used_id);
            Character& pc = _state.party.player();

            _state.party.inventory.remove(used_id, 1);

            if (idef.hp_restore > 0)
            {
                pc.hp += idef.hp_restore;
                int hmax = pc.eff_hp_max();
                if (pc.hp > hmax) pc.hp = hmax;
                render_all();
                render_status("HP restored!");
            }
            else if (idef.mp_restore > 0)
            {
                pc.mp += idef.mp_restore;
                int mmax = pc.eff_mp_max();
                if (pc.mp > mmax) pc.mp = mmax;
                render_all();
                render_status("MP restored!");
            }
            else if (idef.cures != 0)
            {
                pc.status &= ~idef.cures;
                render_all();
                render_status("Status cured!");
            }
            else
            {
                render_all();
                render_status("Used item.");
            }

            _phase = Phase::ENEMY_TURN;
            _delay = 30;
        }
    }
    else if (_phase == Phase::ENEMY_TURN)
    {
        Character& pc = _state.party.player();

        // Enemy status effects
        if (_enemy_status & static_cast<uint8_t>(StatusFlag::POISON))
        {
            _enemy_hp -= 3;
            if (_enemy_hp < 0) _enemy_hp = 0;
        }
        if (_enemy_status & static_cast<uint8_t>(StatusFlag::BURN))
        {
            _enemy_hp -= 5;
            if (_enemy_hp < 0) _enemy_hp = 0;
        }
        if (_enemy_hp <= 0)
        {
            int xp = enemy_stats(_enemy_type).xp_reward;
            pc.gain_xp(xp);
            render_all();
            render_status("Victory! XP gained.");
            _delay = 90;
            _phase = Phase::VICTORY;
            return CombatResult::ONGOING;
        }

        // Check if enemy is paralyzed or sleeping - skip attack
        bool enemy_skip = false;
        if (_enemy_status & static_cast<uint8_t>(StatusFlag::PARALYZE))
            enemy_skip = rng_next(0, 1) == 0;
        if (_enemy_status & static_cast<uint8_t>(StatusFlag::SLEEP))
            enemy_skip = true;

        if (!enemy_skip)
        {
            int dmg = roll_damage(enemy_stats(_enemy_type).atk, pc.eff_def());
            pc.hp -= dmg;
            if (pc.hp < 0) pc.hp = 0;
            render_all();
            render_status("Enemy attacks!");
        }
        else
        {
            render_all();
            render_status("Enemy is stunned!");
        }

        if (pc.hp <= 0)
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

void CombatScene::tick_status_effects()
{
    Character& pc = _state.party.player();

    if (pc.has_status(StatusFlag::POISON))
    {
        pc.hp -= 3;
        if (pc.hp < 0) pc.hp = 0;
        render_status("Poison hurts!");
        _delay = 20;
    }
    if (pc.has_status(StatusFlag::BURN))
    {
        pc.hp -= 5;
        if (pc.hp < 0) pc.hp = 0;
        render_status("Burn hurts!");
        _delay = 20;
    }
    if (pc.has_status(StatusFlag::PARALYZE))
    {
        if (rng_next(0, 1) == 0)
        {
            render_status("Paralyzed! Can't move.");
            _delay = 45;
            _phase = Phase::ENEMY_TURN;
        }
    }
    if (pc.has_status(StatusFlag::SLEEP))
    {
        render_status("You are asleep...");
        _delay = 45;
        _phase = Phase::ENEMY_TURN;
    }
}

// ── Helpers: number append ─────────────────────────────────────────────────────

template<int N>
static void append_int(bn::string<N>& s, int v)
{
    if (v < 0) { s.push_back('-'); v = -v; }
    if (v >= 100) s.push_back('0' + v / 100);
    if (v >= 10)  s.push_back('0' + (v / 10) % 10);
    s.push_back('0' + v % 10);
}

// ── render_all ─────────────────────────────────────────────────────────────────

void CombatScene::render_all()
{
    _sprites.clear();
    const Character& pc = _state.party.player();

    _gen.set_center_alignment();

    // Enemy name + HP
    {
        const char* ename = enemy_stats(_enemy_type).name;
        bn::string<32> title;
        title.append(bn::string_view("VS "));
        for (int i = 0; ename[i]; ++i) title.push_back(ename[i]);
        _gen.generate(0, -72, title, _sprites);
    }

    // Enemy HP bar text
    {
        _gen.set_right_alignment();
        bn::string<32> ehp;
        ehp.append(bn::string_view("HP "));
        append_int(ehp, _enemy_hp);
        ehp.push_back('/');
        append_int(ehp, _enemy_hp_max);
        // Enemy status flags
        if (_enemy_status & static_cast<uint8_t>(StatusFlag::POISON))  ehp.append(bn::string_view("[P]"));
        if (_enemy_status & static_cast<uint8_t>(StatusFlag::BURN))    ehp.append(bn::string_view("[B]"));
        if (_enemy_status & static_cast<uint8_t>(StatusFlag::SILENCE)) ehp.append(bn::string_view("[X]"));
        _gen.generate(116, -48, ehp, _sprites);
    }

    // Player HP / MP
    {
        _gen.set_left_alignment();
        bn::string<40> php;
        php.append(bn::string_view("HP "));
        append_int(php, pc.hp);
        php.push_back('/');
        append_int(php, pc.eff_hp_max());
        php.append(bn::string_view("  MP "));
        append_int(php, pc.mp);
        php.push_back('/');
        append_int(php, pc.eff_mp_max());
        _gen.generate(-116, 36, php, _sprites);
    }

    // Player status flags
    if (pc.status != 0)
    {
        _gen.set_left_alignment();
        bn::string<24> sflag;
        if (pc.has_status(StatusFlag::POISON))  sflag.append(bn::string_view("P "));
        if (pc.has_status(StatusFlag::BURN))    sflag.append(bn::string_view("B "));
        if (pc.has_status(StatusFlag::PARALYZE))sflag.append(bn::string_view("Z "));
        if (pc.has_status(StatusFlag::SLEEP))   sflag.append(bn::string_view("S "));
        if (pc.has_status(StatusFlag::SILENCE)) sflag.append(bn::string_view("X "));
        if (!sflag.empty())
            _gen.generate(-116, 48, sflag, _sprites);
    }

    // Menu based on phase
    if (_phase == Phase::PLAYER_TURN)
        render_main_menu();
    else if (_phase == Phase::MAGIC_SELECT)
        render_magic_menu();
    else if (_phase == Phase::ITEM_SELECT)
        render_item_menu();
}

void CombatScene::render_main_menu()
{
    // 2-column layout
    // col0 x=-116, col1 x=0
    // row0 y=56: Attack (0), Magic (1)
    // row1 y=70: Item (2), Flee (3)
    const int cx[2] = { -116, 0 };
    const int ry[2] = { 56, 70 };

    _gen.set_left_alignment();
    const char* labels[4] = { "Attack", "Magic", "Item", "Flee" };
    for (int i = 0; i < 4; ++i)
    {
        int col = i % 2;
        int row = i / 2;
        bn::string<16> line;
        if (_cursor == i)
            line.append(bn::string_view("> "));
        else
            line.append(bn::string_view("  "));
        const char* lbl = labels[i];
        for (int j = 0; lbl[j]; ++j) line.push_back(lbl[j]);
        _gen.generate(cx[col], ry[row], line, _sprites);
    }
}

void CombatScene::render_magic_menu()
{
    const Character& pc = _state.party.player();

    _gen.set_center_alignment();
    _gen.generate(0, 36, bn::string_view("-- Spells --"), _sprites);

    _gen.set_left_alignment();
    for (int i = 0; i <= pc.spell_count; ++i)
    {
        int y = 44 + i * 14;
        bn::string<24> line;
        if (_sub_cursor == i)
            line.push_back('>');
        else
            line.push_back(' ');
        line.push_back(' ');

        if (i == pc.spell_count)
        {
            line.append(bn::string_view("Back"));
        }
        else
        {
            const SpellDef& sp = spell_def(pc.known_spells[i]);
            const char* sname = sp.name;
            for (int j = 0; sname[j]; ++j) line.push_back(sname[j]);
            // Pad to column then show mp cost
            while ((int)line.size() < 14) line.push_back(' ');
            append_int(line, sp.mp_cost);
            line.append(bn::string_view("mp"));
        }
        _gen.generate(-116, y, line, _sprites);
    }
}

void CombatScene::render_item_menu()
{
    _gen.set_center_alignment();
    _gen.generate(0, 36, bn::string_view("-- Items --"), _sprites);

    // Build usable list
    ItemId usable_ids[8] = {};
    int usable_count = 0;
    for (int i = 0; i < _state.party.inventory.count && usable_count < 8; ++i)
    {
        ItemId iid = _state.party.inventory.slots[i].id;
        if (iid != ItemId::NONE && item_def(iid).combat_usable)
            usable_ids[usable_count++] = iid;
    }

    _gen.set_left_alignment();
    for (int i = 0; i <= usable_count; ++i)
    {
        int y = 44 + i * 14;
        bn::string<24> line;
        if (_sub_cursor == i)
            line.push_back('>');
        else
            line.push_back(' ');
        line.push_back(' ');

        if (i == usable_count)
        {
            line.append(bn::string_view("Back"));
        }
        else
        {
            const ItemDef& idef = item_def(usable_ids[i]);
            const char* iname = idef.name;
            for (int j = 0; iname[j]; ++j) line.push_back(iname[j]);
            line.push_back(' ');
            line.push_back('x');
            int qty = _state.party.inventory.qty_of(usable_ids[i]);
            line.push_back('0' + qty % 10);
        }
        _gen.generate(-116, y, line, _sprites);
    }
}

void CombatScene::render_status(const char* msg)
{
    _sprites.clear();
    render_all();
    _gen.set_center_alignment();
    bn::string<48> s;
    for (int i = 0; msg[i]; ++i) s.push_back(msg[i]);
    _gen.generate(0, -56, s, _sprites);
}
