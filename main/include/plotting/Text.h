#pragma once
#include <agge.text/font.h>
#include <agge.text/layout.h>
#include <agge.text/limit.h>

#include <plotting/types/text_engine.h>
#include <plotting/canvas.h>

namespace plotting
{
    namespace inner
    {
        //int align_to_int(agge::text_alignment )
        inline agge::rect_r text_destination(agge::point_r pos, agge::box_r hw, agge::full_alignment al)
        {
            agge::rect_r dest{pos.x - hw.w*0.5f, pos.y - hw.h*0.5f,
                  pos.x + hw.w*0.5f, pos.y + hw.h*0.5f};
            agge::vector_r shift{hw.w*0.5f*agge::real_t(al.halign), hw.h*0.5f*agge::real_t(al.valign)};
            dest += shift;
            return dest;
        }
    }
    struct TextProperties
    {
        agge::font_descriptor _font = agge::font_descriptor::create("Fira Code", 20);
        agge::full_alignment  _align;
        agge::color           _color{255,255,255,128};
        agge::color           _fill{0,0,0,0};

        static TextProperties from(agge::full_alignment al)
        {
            return {agge::font_descriptor::create("Fira Code", 20), al, {255,255,255,128}, {0,0,0,0}};
        }

        TextProperties& font(agge::font_descriptor font) { _font = font; return *this; }
        TextProperties& horizontal(agge::text_alignment h) { _align.halign = h; return *this; }
        TextProperties& vertical(agge::text_alignment v) { _align.valign = v; return *this; }
        TextProperties& horizontal(agge::full_alignment hv) { _align = hv; return *this; }

        TextProperties& color(agge::color c) { _color = c; return *this; }
        TextProperties& fill(agge::color c) { _fill = c; return *this; }
        TextProperties& contrast(agge::color c) { _color = c, _fill = c.inverted(); return *this; }
    };

    class Text
    {
    public:
        Text(TextProperties p)
            : _text({p._font,})
            , _align{p._align}
            , _color{p._color}
            , _fill{p._fill}
        {}

        Text(agge::font_descriptor font)
            : _text({font,})
        {}

        Text(agge::font_style_annotation font)
            : _text(font)
        {}

        Text& font(agge::font_descriptor font) { _text.set_base_annotation({font,}); return *this; }
        Text& font(agge::font_descriptor font, agge::color col)
        {
            _text.set_base_annotation({font, col}); return *this;
        }
        Text& font(agge::font_style_annotation font) { _text.set_base_annotation(font); return *this; }


        Text& text(char const* text)
        {
            _text.clear(); _text << text; return *this;
        }

        Text& text(std::string const& text)
        {
            _text.clear(); _text << text.c_str(); return *this;
        }

        Text& position(agge::point_r const& pos) { _position = pos; return *this; }
        Text& horizontal(agge::text_alignment h) { _align.halign = h; return *this; }
        Text& vertical(agge::text_alignment v) { _align.valign = v; return *this; }
        Text& align(agge::full_alignment hv) { _align = hv; return *this; }

        Text& color(agge::color c) { _color = c; return *this; }
        Text& fill(agge::color c) { _fill = c; return *this; }
        Text& contrast(agge::color c) { _color = c, _fill = c.inverted(); return *this; }

        agge::richtext_t     _text;
        agge::point_r        _position{};
        agge::full_alignment _align{agge::align_near, agge::align_near};
        agge::color          _color{255, 255, 255, 128};
        agge::color          _fill{0, 0, 0, 0};
    };

    template<typename S, typename Rd, typename Rs>
    Canvas<S, Rd, Rs>& operator<<(Canvas<S, Rd, Rs>& c, Text const& t)
    {
        auto& text_engine = plotting::global::get_text_engine();
        auto& layout = plotting::global::get_layout();
        float const max_width = 100.0f;
        auto wrap = agge::limit::wrap(t._position.x + max_width);
        auto hw = text_engine.measure(t._text, wrap);
        auto dest = inner::text_destination(t._position, hw, t._align);
        c << agge::rectangle(dest.x1, dest.y1, dest.x2, dest.y2) << t._fill;
        layout.process(t._text, wrap, text_engine);
        text_engine.render(c.ras, layout, agge::align_center, agge::align_center, dest);
        c.ras.sort(true);
        c.ren(c.surface, agge::zero(), 0, c.ras
            , agge::platform_blender_solid_color(t._color), agge::winding<>());
        c.ras.reset();
        return c;
    }
}