﻿#include <agge/rendering/clipper.h>
#include <agge/rendering/renderer.h>
#include <agge/rendering/rasterizer.h>
#include <agge/rendering/filling_rules.h>

#include <agge/primitives/curves.h>
#include <agge/primitives/dash.h>
#include <agge/primitives/polyline.h>
#include <agge/primitives/figures.h>
#include <agge/primitives/path.h>
#include <agge/primitives/stroke.h>
#include <agge/primitives/stroke_features.h>

#include <plotting/Axes.h>
#include <plotting/Canvas.h>
#include <plotting/types/text_engine.h>

#include <agge.text/layout.h>
#include <agge.text/limit.h>

#include <samples/common/shell.h>

#include <algorithm>

namespace
{
    template <typename T>
    agge::rect<T> mkrect(T x1, T y1, T x2, T y2)
    {
        return {x1, y1, x2, y2};
    }

    class Plotting: public application
    {
    public:
        Plotting()
            : application()
        {
            axes.coordinates.repr_area = mkrect(-6.5, 20., 6.2, -20.);
        }
    private:

        virtual void draw(agge::platform_bitmap& surface, timings&/*timings*/)
        {
            fill(surface, mkrect<int>(0, 0, surface.width(), surface.height()),
                 agge::platform_blender_solid_color(agge::color::make(10, 10, 10)));

            auto canvas = plotting::make_canvas(surface, ren, ras);

            canvas << axes;

            auto wid = float(surface.width());
            auto hei = float(surface.height());
            auto scale = std::min(wid/400.0f, hei/370.0f);

            line_style.width(1.0f*scale);
            line_style.set_cap(agge::caps::butt());
            line_style.set_join(agge::joins::bevel());
            dash_style.remove_all_dashes();
            dash_style.add_dash(10.0f*scale, 5.0f*scale);
            dash_style.dash_start(0.5f*scale);

            canvas << plotting::reset
                << agge::polyline_adaptor(points1)/dash_style/line_style
                << agge::color::make(255, 0, 0, 128);

            canvas << plotting::reset
                << agge::polyline_adaptor(points2)/line_style
                << agge::color::make(0, 255, 0, 128);

            agge::rect_r dest = {surface.width()*0.5f, surface.height()*0.5f, surface.width()*0.5f, surface.height()*0.5f};

            agge::font_style_annotation a = {agge::font_descriptor::create("Times New Roman", 30, agge::regular, false, agge::hint_none),};
            agge::richtext_t _text(a);
            _text.set_base_annotation(a);
            _text << "Helloo, Text!";
            auto& _text_engine = plotting::get_text_engine();
            auto& _layout = plotting::get_layout();
            _layout.process(_text, agge::limit::wrap(dest.x2), _text_engine);
            _text_engine.render(ras, _layout, agge::align_near, agge::align_near, dest);
            ras.sort(true);
            ren(surface, agge::zero(), 0, ras
                , agge::platform_blender_solid_color(agge::color::make(255, 0, 0)), agge::winding<>());

            ras.reset();
        }

        virtual void resize(int width, int height)
        {
            auto const F = [](double t) { return cos(t)*exp(0.5*t); };
            axes.position = plotting::port_area_t{10.0f, 10.0f, (float)width-10.0f, (float)height-10.0f};
            axes.coordinates.update(plotting::port_area_t{100.0f, 40.0f, (float)width-20.0f, (float)height-100.0f});

            //axes.alignTickStepsX1(150);
            //axes.alignTickStepsX2(300);
            //axes.alignTickStepsY1(150);
            //axes.alignTickStepsY2(300);

            {
                double t = -5.;
                plotting::port_t cur;
                plotting::port_t prev = axes.coordinates << plotting::repr_t{t, F(t)};
                points1.clear();
                points1.move_to(prev.x, prev.y);
                for(t = -5; t < 5.; t += 0.01, prev = cur)
                {
                    cur = axes.coordinates << plotting::repr_t{t, F(t)};
                    points1.line_to(cur.x, cur.y);
                }
            }
            {
                points2.clear();
                points2.move_to(axes.coordinates << plotting::repr_t{-5., 10.});
                points2.line_to(axes.coordinates << plotting::repr_t{5., -10.});
            }
        }

    private:
        plotting::Axes             axes;
        plotting::rasterizer       ras;
        agge::renderer             ren;
        agge::stroke               line_style;
        agge::dash                 dash_style;
        agge::polyline             points1;
        agge::polyline             points2;
    };
}

application* agge_create_application(services&/*s*/)
{
    return new Plotting;
}
