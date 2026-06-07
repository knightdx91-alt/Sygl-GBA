#pragma once
#include "bn_vector.h"
#include "bn_sprite_ptr.h"
#include "bn_sprite_text_generator.h"
#include "bn_string_view.h"
#include "bn_array.h"

// Simple two-line dialogue box displayed in the lower portion of the screen.
// Caller fills pages via open(); advance() moves to the next page or closes.
class TextBox
{
public:
    static constexpr int MAX_PAGES = 8;
    static constexpr int LINES_PER_PAGE = 2;

    TextBox();

    void open(bn::sprite_text_generator& gen,
              const bn::string_view* lines, int line_count);

    // Call once per frame. Returns true while box is visible.
    bool update(bn::sprite_text_generator& gen);

    bool is_open() const { return _open; }

    // Advance to next page, or close if on last page.
    void advance(bn::sprite_text_generator& gen);

private:
    struct Page
    {
        bn::string_view line[LINES_PER_PAGE];
        int line_count = 0;
    };

    void render_current_page(bn::sprite_text_generator& gen);

    bn::vector<bn::sprite_ptr, 128> _sprites;
    Page _pages[MAX_PAGES];
    int _page_count = 0;
    int _current_page = 0;
    int _visible_chars = 0;
    int _frame_timer = 0;
    bool _open = false;
};
