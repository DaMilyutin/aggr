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
#include <plotting/Canvas.h>

#include <samples/common/shell.h>

#include <algorithm>

using namespace agge;

namespace
{
    template <typename T>
    rect<T> mkrect(T x1, T y1, T x2, T y2)
    {
        rect<T> r = {x1, y1, x2, y2};
        return r;
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
                 agge::platform_blender_solid_color(color::make(10, 10, 10)));

            auto canvas = plotting::make_canvas(surface, ren, ras);

            canvas << axes;

            auto wid = float(surface.width());
            auto hei = float(surface.height());
            auto scale = std::min(wid/400.0f, hei/370.0f);

            line_style.width(1.0f*scale);
            line_style.set_cap(caps::butt());
            line_style.set_join(joins::bevel());
            dash_style.remove_all_dashes();
            dash_style.add_dash(10.0f*scale, 5.0f*scale);
            dash_style.dash_start(0.5f*scale);

            canvas << plotting::reset
                << agge::polyline_adaptor(points1)/dash_style/line_style
                << platform_blender_solid_color(color::make(255, 0, 0, 128));

            canvas << plotting::reset
                << agge::polyline_adaptor(points2)/line_style
                << platform_blender_solid_color(color::make(0, 255, 0, 128));
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
        rasterizer< clipper<int> > ras;
        renderer                   ren;
        stroke                     line_style;
        dash                       dash_style;
        agge::polyline             points1;
        agge::polyline             points2;
    };
}

application* agge_create_application(services&/*s*/)
{
    return new Plotting;
}
