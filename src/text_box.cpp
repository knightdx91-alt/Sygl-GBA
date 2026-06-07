#include "text_box.h"
#include "bn_string.h"

static constexpr int BOX_Y1 = 44;   // top of box (pixels from center)
static constexpr int LINE1_Y = 50;
static constexpr int LINE2_Y = 66;
static constexpr int CHARS_PER_FRAME = 1; // typewriter speed (chars per 2 frames)

TextBox::TextBox() = default;

void TextBox::open(bn::sprite_text_generator& gen,
                   const bn::string_view* lines, int line_count)
{
    _page_count = 0;
    _current_page = 0;
    _visible_chars = 0;
    _frame_timer = 0;
    _open = true;

    // Pack lines two per page
    int li = 0;
    while (li < line_count && _page_count < MAX_PAGES)
    {
        Page& p = _pages[_page_count++];
        p.line_count = 0;
        for (int j = 0; j < LINES_PER_PAGE && li < line_count; ++j, ++li)
        {
            p.line[j] = lines[li];
            p.line_count++;
        }
    }

    render_current_page(gen);
}

bool TextBox::update(bn::sprite_text_generator& gen)
{
    if (!_open) return false;

    _frame_timer++;
    if (_frame_timer >= 2)
    {
        _frame_timer = 0;
        const Page& p = _pages[_current_page];
        int total_chars = 0;
        for (int i = 0; i < p.line_count; ++i)
            total_chars += (int)p.line[i].size();

        if (_visible_chars < total_chars)
        {
            _visible_chars += CHARS_PER_FRAME;
            if (_visible_chars > total_chars) _visible_chars = total_chars;
            render_current_page(gen);
        }
    }

    return true;
}

void TextBox::advance(bn::sprite_text_generator& gen)
{
    if (!_open) return;

    const Page& p = _pages[_current_page];
    int total_chars = 0;
    for (int i = 0; i < p.line_count; ++i)
        total_chars += (int)p.line[i].size();

    // If still animating, snap to full text first
    if (_visible_chars < total_chars)
    {
        _visible_chars = total_chars;
        render_current_page(gen);
        return;
    }

    // Move to next page or close
    _current_page++;
    if (_current_page >= _page_count)
    {
        _open = false;
        _sprites.clear();
    }
    else
    {
        _visible_chars = 0;
        _frame_timer = 0;
        render_current_page(gen);
    }
}

void TextBox::render_current_page(bn::sprite_text_generator& gen)
{
    _sprites.clear();
    if (_current_page >= _page_count) return;

    const Page& p = _pages[_current_page];
    gen.set_left_alignment();

    int chars_remaining = _visible_chars;
    int y_positions[LINES_PER_PAGE] = { LINE1_Y, LINE2_Y };

    for (int i = 0; i < p.line_count && chars_remaining > 0; ++i)
    {
        int len = (int)p.line[i].size();
        int show = chars_remaining < len ? chars_remaining : len;

        // Build a substr-like view
        bn::string<40> visible_text;
        for (int c = 0; c < show; ++c)
            visible_text.push_back(p.line[i][c]);

        gen.generate(-110, y_positions[i], visible_text, _sprites);
        chars_remaining -= len;
    }

    // "Press A" prompt when page is fully revealed
    int total_chars = 0;
    for (int i = 0; i < p.line_count; ++i)
        total_chars += (int)p.line[i].size();

    if (_visible_chars >= total_chars)
    {
        gen.set_right_alignment();
        gen.generate(110, LINE2_Y, bn::string_view("Press A"), _sprites);
        gen.set_left_alignment();
    }
}
