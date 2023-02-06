#include <agge/rendering/clipper.h>
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
#include <plotting/Chart.h>

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
            axes.coordinates.repr_area = mkrect(-5., 2.0, 5., -2.0);
            line_style.set_cap(agge::caps::butt());
            line_style.set_join(agge::joins::bevel());

            axes.properties.y1.tickSteps = 5;
            axes.properties.y1.tickGap = 10;

            axes.properties.y2.tickSteps = 5;
            axes.properties.y2.tickGap = 10;

            //auto t = std::chrono::system_clock::now();
            //std::chrono::time_point days = std::chrono::round<std::chrono::days>(t);
            //axes.properties.x2.tickSteps = 20;
            //axes.properties.x2.tickGap = 120;
            axes.properties.x2.labels.base.vertical(agge::align_far);
            //axes.properties.x2.labels.format = [days, p = std::chrono::days(std::numeric_limits<std::chrono::days::rep>::min())](double x) mutable
            //{
            //    std::chrono::time_point t = days + std::chrono::round<std::chrono::seconds>(std::chrono::duration<double>(x*86400.*0.125));
            //    auto c = std::chrono::floor<std::chrono::days>(t.time_since_epoch());
            //    std::string ret;
            //    constexpr auto undef = std::chrono::days(std::numeric_limits<std::chrono::days::rep>::min());
            //    if(c == p /*|| p == undef*/)
            //        ret = std::format("{:%T}", t);
            //    else
            //        ret = std::format("{:%T\n%F}", t);
            //    p = c;
            //    return ret;
            //};

            axes.properties.x2.grid.color = agge::color{255, 255, 0, 128};
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
            auto const F = [](double t) { return sin(t/(t*t+0.03))*exp(0.5*t); };
            axes.position = plotting::port_area_t{10.0f, 10.0f, (float)width-10.0f, (float)height-10.0f};
            axes.coordinates.update(plotting::port_area_t{100.0f, 40.0f, (float)width-120.0f, (float)height-100.0f});

            plotting::Function gen{{}, -5., 5., 0.01, {F}};

            //{
            //    double t = -5.;
            //    plotting::port_t cur;
            //    plotting::port_t prev = axes.coordinates << plotting::repr_t{t, F(t)};
            //    points1.clear();
            //    points1.move_to(prev.x, prev.y);
            //    for(t = -5; t < 5.; t += 0.01, prev = cur)
            //    {
            //        cur = axes.coordinates << plotting::repr_t{t, F(t)};
            //        points1.line_to(cur.x, cur.y);
            //    }
            //}
            points1 << agge::clear << gen/axes.coordinates/filter(plotting::filters::FarEnough{50.0f});
            points2 << agge::clear << plotting::repr_t{-5., 10.}/axes.coordinates
                                   << plotting::repr_t{5., -10.}/axes.coordinates;
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
