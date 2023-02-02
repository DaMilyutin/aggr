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

        void drawAxesArea(platform_bitmap& surface)
        {
            ras.reset();
            ras.set_clipping(axes.position);
            //axes.properties.x1;
            line_style.width(axes.properties.x1.sep.width);
            line_style.set_cap(caps::butt());
            line_style.set_join(joins::bevel());
            add_path(ras,
                      agge::line(axes.coordinates.port_area.x1, axes.coordinates.port_area.y1,
                                 axes.coordinates.port_area.x1, axes.coordinates.port_area.y2)
                                 / line_style);
            ras.sort();

            ren(surface, zero(), 0 /*no windowing*/, ras /*mask*/,
                platform_blender_solid_color(axes.properties.x1.sep.color), winding<>());
            ras.reset();

            line_style.width(axes.properties.x2.sep.width);
            line_style.set_cap(caps::butt());
            line_style.set_join(joins::bevel());
            add_path(ras,
                agge::line(axes.coordinates.port_area.x2, axes.coordinates.port_area.y1,
                           axes.coordinates.port_area.x2, axes.coordinates.port_area.y2)
                           / line_style);
            ras.sort();

            ren(surface, zero(), 0 /*no windowing*/, ras /*mask*/,
                platform_blender_solid_color(axes.properties.x2.sep.color), winding<>());
            ras.reset();

            {
                double step_repr = (axes.coordinates.repr_area.x2 - axes.coordinates.repr_area.x1)/axes.properties.x1.tickSteps;
                step_repr = floor(log(step_repr)/log(10.));
                step_repr = exp(step_repr*log(10.));
                double const X_repr = step_repr*ceil(axes.coordinates.repr_area.x1/step_repr);
                real_t const x1 = real_t(axes.coordinates.scale.x*(X_repr-axes.coordinates.repr_area.x1) + axes.coordinates.port_area.x1);
                real_t const step = real_t(step_repr*axes.coordinates.scale.x);

                real_t const x2 = axes.coordinates.port_area.x2;

                auto major = [&](real_t const y1, real_t const y2,
                                                        plotting::Axes::TickProperties const& tick)
                {
                    ras.reset();
                    line_style.width(tick.width);
                    line_style.set_cap(caps::butt());
                    line_style.set_join(joins::bevel());
                    for(real_t x = x1; x<=x2; x += step)
                        add_path(ras, agge::line(x, y1, x, y2)/line_style);

                    ras.sort();
                    ren(surface, zero(), 0 /*no windowing*/, ras /*mask*/,
                        platform_blender_solid_color(tick.color), winding<>());
                };

                auto middle = [&](real_t const y1, real_t const y2,
                    plotting::Axes::TickProperties const& tick)
                {
                    ras.reset();
                    line_style.width(tick.width);
                    line_style.set_cap(caps::butt());
                    line_style.set_join(joins::bevel());
                    real_t x = x1 - step*0.5f;
                    while(x < axes.coordinates.port_area.x1)
                        x += step;
                    for(; x<=x2; x += step)
                        add_path(ras, agge::line(x, y1, x, y2)/line_style);

                    ras.sort();
                    ren(surface, zero(), 0 /*no windowing*/, ras /*mask*/,
                        platform_blender_solid_color(tick.color), winding<>());
                };

                auto minor = [&](real_t const y1, real_t const y2, real_t step,
                    plotting::Axes::TickProperties const& tick)
                {
                    step *= 0.1f;
                    ras.reset();
                    line_style.width(tick.width);
                    line_style.set_cap(caps::butt());
                    line_style.set_join(joins::bevel());
                    int i = 1;
                    real_t x = x1 - step*9.0f;
                    while(x < axes.coordinates.port_area.x1)
                        x += step, ++i;
                    for(; x<=x2; x += step)
                    {
                        if(i++ % 5 == 0)
                            continue;
                        add_path(ras, agge::line(x, y1, x, y2)/line_style);
                    }

                    ras.sort();
                    ren(surface, zero(), 0 /*no windowing*/, ras /*mask*/,
                        platform_blender_solid_color(tick.color), winding<>());
                };


                {
                    real_t const y2 = axes.coordinates.port_area.y1;
                    real_t const y1 = axes.coordinates.port_area.y1 - axes.properties.y1.tick[0].length;
                    major(y1, y2, axes.properties.y1.tick[0]);
                }
                {
                    real_t const y1 = axes.coordinates.port_area.y2;
                    real_t const y2 = axes.coordinates.port_area.y2 + axes.properties.y2.tick[0].length;
                    major(y1, y2, axes.properties.y2.tick[0]);
                }
                {
                    real_t const y2 = axes.coordinates.port_area.y1;
                    real_t const y1 = axes.coordinates.port_area.y1 - axes.properties.y1.tick[1].length;
                    middle(y1, y2, axes.properties.y1.tick[1]);
                }
                {
                    real_t const y1 = axes.coordinates.port_area.y2;
                    real_t const y2 = axes.coordinates.port_area.y2 + axes.properties.y2.tick[1].length;
                    middle(y1, y2, axes.properties.y2.tick[1]);
                }
                {
                    real_t const y2 = axes.coordinates.port_area.y1;
                    real_t const y1 = axes.coordinates.port_area.y1 - axes.properties.y1.tick[2].length;
                    minor(y1, y2, step, axes.properties.y1.tick[2]);
                }
                {
                    real_t const y1 = axes.coordinates.port_area.y2;
                    real_t const y2 = axes.coordinates.port_area.y2 + axes.properties.y2.tick[2].length;
                    minor(y1, y2, step, axes.properties.y2.tick[2]);
                }

            }

            line_style.width(axes.properties.y1.sep.width);
            line_style.set_cap(caps::butt());
            line_style.set_join(joins::bevel());
            auto canvas = plotting::make_canvas(surface, ren, ras);

            canvas << plotting::reset
                   << agge::line(axes.coordinates.port_area.x1, axes.coordinates.port_area.y1,
                                  axes.coordinates.port_area.x2, axes.coordinates.port_area.y1)
                                 / line_style
                   << platform_blender_solid_color(axes.properties.y1.sep.color);

            ras.reset();
            line_style.width(axes.properties.y2.sep.width);
            line_style.set_cap(caps::butt());
            line_style.set_join(joins::bevel());
            add_path(ras,
                     agge::line(axes.coordinates.port_area.x1, axes.coordinates.port_area.y2,
                                axes.coordinates.port_area.x2, axes.coordinates.port_area.y2)
                        /line_style);
            ras.sort();

            ren(surface, zero(), 0 /*no windowing*/, ras /*mask*/,
                platform_blender_solid_color(axes.properties.y2.sep.color), winding<>());
            ras.reset();
        }

        virtual void draw(platform_bitmap& surface, timings&/*timings*/)
        {
            fill(surface, mkrect<int>(0, 0, surface.width(), surface.height()),
                platform_blender_solid_color(color::make(10, 10, 10)));

            drawAxesArea(surface);

            ras.set_clipping(axes.coordinates.port_area);

            auto wid = float(surface.width());
            auto hei = float(surface.height());
            auto scale = std::min(wid/400.0f, hei/370.0f);

            line_style.width(1.0f*scale);
            line_style.set_cap(caps::butt());
            line_style.set_join(joins::bevel());
            dash_style.remove_all_dashes();
            dash_style.add_dash(10.0f*scale, 5.0f*scale);
            dash_style.dash_start(0.5f*scale);

            auto canvas = plotting::make_canvas(surface, ren, ras);

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
