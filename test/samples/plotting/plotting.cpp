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
#include <plotting/primitives/Canvas.h>
#include <plotting/primitives/Text.h>

#include <agge.text/layout.h>
#include <agge.text/limit.h>

#include <samples/common/shell.h>

#include <algorithm>
#include <chrono>

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
            axes.coordinates.repr_area = mkrect(-0.05, 1.0, 0.1, -1.0);
            line_style.set_cap(agge::caps::butt());
            line_style.set_join(agge::joins::bevel());

            auto t = std::chrono::system_clock::now();
            std::chrono::time_point days = std::chrono::round<std::chrono::days>(t);
            axes.properties.x2.tickSteps = 4;
            axes.properties.x2.tickGap = 160;
            axes.properties.x2.labels.base.vertical(agge::align_far);
            axes.properties.x2.labels.format = [days](double x) {
                std::chrono::time_point t =  days + std::chrono::seconds{std::chrono::seconds::rep(x*86400.)};
                return std::format("{:%Y-%m-%d}\n{:%H:%M}", t, t);
            };
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
            dash_style.remove_all_dashes();
            dash_style.add_dash(10.0f*scale, 5.0f*scale);
            dash_style.dash_start(0.5f*scale);

            canvas.ras.set_clipping(axes.coordinates.port_area);
            canvas << plotting::reset
                << agge::polyline_adaptor(points1)/dash_style/line_style
                << agge::color::make(255, 0, 0, 128);

            canvas << plotting::reset
                << agge::polyline_adaptor(points2)/line_style
                << agge::color::make(0, 255, 0, 128);

            plotting::Text hello(agge::font_descriptor::create("Times New Roman", 25,
                agge::regular, false, agge::hint_none));
        }

        virtual void resize(int width, int height)
        {
            auto const F = [](double t) { return cos(1./(fabs(t)+1.))*exp(0.5*t); };
            axes.position = plotting::port_area_t{10.0f, 10.0f, (float)width-10.0f, (float)height-10.0f};
            axes.coordinates.update(plotting::port_area_t{100.0f, 40.0f, (float)width-120.0f, (float)height-100.0f});

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
