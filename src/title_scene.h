#pragma once
#include "bn_vector.h"
#include "bn_sprite_ptr.h"
#include "bn_sprite_text_generator.h"

class TitleScene
{
public:
    explicit TitleScene(bool has_save);

    // Returns true when the player has chosen New Game.
    // Returns false when the player has chosen Continue (load save).
    // Call update() in a loop and check done() + result() separately.
    void update();

    bool done() const { return _done; }
    bool chose_new_game() const { return _chose_new_game; }

private:
    void render(bn::sprite_text_generator& gen);

    bn::sprite_text_generator _gen;
    bn::vector<bn::sprite_ptr, 128> _sprites;
    bool _has_save;
    int _cursor;       // 0 = New Game, 1 = Continue
    bool _done;
    bool _chose_new_game;
};
