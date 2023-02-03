#pragma once
#include <agge.text/font.h>
#include <agge.text/layout.h>
#include <agge.text/limit.h>

#include <plotting/types/text_engine.h>
#include <plotting/canvas.h>

namespace plotting
{
    class Text
    {
    public:
        Text() = default;
        Text(agge::font_descriptor font)
            : _text({font,})
        {}

        Text(agge::font_style_annotation font)
            : _text(font)
        {}

        Text& font(agge::font_descriptor font) { _text.set_base_annotation({font,}); return *this; }
        Text& font(agge::font_descriptor font, agge::color col)
        { _text.set_base_annotation({font, col}); return *this; }
        Text& font(agge::font_style_annotation font) { _text.set_base_annotation(font); return *this; }


        Text& text(char const* text)
        {
            _text.clear(); _text << text; return *this;
        }

        Text& text(std::string const& text)
        {
            _text.clear(); _text << text.c_str(); return *this;
        }

        Text& position(agge::rect_r const& pos) { _dest = pos; return *this; }
        Text& horizontal(agge::text_alignment h){ _horizontal = h; return *this; }
        Text& vertical(agge::text_alignment v) { _vertical = v; return *this; }
        Text& fill(agge::color c) { _fill = c; return *this; }

        agge::richtext_t _text;
        agge::rect_r     _dest;
        agge::text_alignment _horizontal = agge::align_near;
        agge::text_alignment _vertical   = agge::align_near;
        agge::color      _fill;
    };

    template<typename S, typename Rd, typename Rs>
    Canvas<S, Rd, Rs>& operator<<(Canvas<S, Rd, Rs>& c, Text const& t)
    {
        auto& text_engine = plotting::global::get_text_engine();
        auto& layout = plotting::global::get_layout();
        layout.process(t._text, agge::limit::wrap(t._dest.x2), text_engine);
        text_engine.render(c.ras, layout, agge::align_near, agge::align_near, t._dest);
        c.ras.sort(true);
        c.ren(c.surface, agge::zero(), 0, c.ras
            , agge::platform_blender_solid_color(t._fill), agge::winding<>());
        c.ras.reset();
        return c;
    }
}