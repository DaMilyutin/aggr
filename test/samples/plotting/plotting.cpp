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
            axes.coord.repr_area = mkrect(-6.5, 20., 6.2, -20.);
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
                      assist(agge::line(axes.coord.port_area.x1, axes.coord.port_area.y1,
                                                   axes.coord.port_area.x1, axes.coord.port_area.y2),
                                  line_style));
            ras.sort();

            ren(surface, zero(), 0 /*no windowing*/, ras /*mask*/,
                platform_blender_solid_color(axes.properties.x1.sep.color), winding<>());
            ras.reset();

            line_style.width(axes.properties.x2.sep.width);
            line_style.set_cap(caps::butt());
            line_style.set_join(joins::bevel());
            add_path(ras,
                assist(agge::line(axes.coord.port_area.x2, axes.coord.port_area.y1,
                                             axes.coord.port_area.x2, axes.coord.port_area.y2),
                    line_style));
            ras.sort();

            ren(surface, zero(), 0 /*no windowing*/, ras /*mask*/,
                platform_blender_solid_color(axes.properties.x2.sep.color), winding<>());
            ras.reset();

            {
                double step_repr = (axes.coord.repr_area.x2 - axes.coord.repr_area.x1)/axes.properties.x1.tickSteps;
                step_repr = floor(log(step_repr)/log(10.));
                step_repr = exp(step_repr*log(10.));
                double const X_repr = step_repr*ceil(axes.coord.repr_area.x1/step_repr);
                real_t const x1 = real_t(axes.coord.scale.x*(X_repr-axes.coord.repr_area.x1) + axes.coord.port_area.x1);
                real_t const step = real_t(step_repr*axes.coord.scale.x);

                real_t const x2 = axes.coord.port_area.x2;

                auto major = [&](real_t const y1, real_t const y2,
                                                        Axes::TickProperties const& tick)
                {
                    ras.reset();
                    line_style.width(tick.width);
                    line_style.set_cap(caps::butt());
                    line_style.set_join(joins::bevel());
                    for(real_t x = x1; x<=x2; x += step)
                    {
                        add_path(ras,
                            assist(agge::line(x, y1, x, y2),
                                line_style));
                    }

                    ras.sort();
                    ren(surface, zero(), 0 /*no windowing*/, ras /*mask*/,
                        platform_blender_solid_color(tick.color), winding<>());
                };

                auto middle = [&](real_t const y1, real_t const y2,
                    Axes::TickProperties const& tick)
                {
                    ras.reset();
                    line_style.width(tick.width);
                    line_style.set_cap(caps::butt());
                    line_style.set_join(joins::bevel());
                    real_t x = x1 - step*0.5f;
                    while(x < axes.coord.port_area.x1)
                        x += step;
                    for(; x<=x2; x += step)
                    {
                        add_path(ras,
                            assist(agge::line(x, y1, x, y2),
                                line_style));
                    }

                    ras.sort();
                    ren(surface, zero(), 0 /*no windowing*/, ras /*mask*/,
                        platform_blender_solid_color(tick.color), winding<>());
                };

                auto minor = [&](real_t const y1, real_t const y2, real_t step,
                    Axes::TickProperties const& tick)
                {
                    step *= 0.1f;
                    ras.reset();
                    line_style.width(tick.width);
                    line_style.set_cap(caps::butt());
                    line_style.set_join(joins::bevel());
                    int i = 1;
                    real_t x = x1 - step*9.0f;
                    while(x < axes.coord.port_area.x1)
                        x += step, ++i;
                    for(; x<=x2; x += step)
                    {
                        if(i++ % 5 == 0)
                            continue;
                        add_path(ras,
                            assist(agge::line(x, y1, x, y2),
                                line_style));
                    }

                    ras.sort();
                    ren(surface, zero(), 0 /*no windowing*/, ras /*mask*/,
                        platform_blender_solid_color(tick.color), winding<>());
                };


                {
                    real_t const y2 = axes.coord.port_area.y1;
                    real_t const y1 = axes.coord.port_area.y1 - axes.properties.y1.tick[0].length;
                    major(y1, y2, axes.properties.y1.tick[0]);
                }
                {
                    real_t const y1 = axes.coord.port_area.y2;
                    real_t const y2 = axes.coord.port_area.y2 + axes.properties.y2.tick[0].length;
                    major(y1, y2, axes.properties.y2.tick[0]);
                }
                {
                    real_t const y2 = axes.coord.port_area.y1;
                    real_t const y1 = axes.coord.port_area.y1 - axes.properties.y1.tick[1].length;
                    middle(y1, y2, axes.properties.y1.tick[1]);
                }
                {
                    real_t const y1 = axes.coord.port_area.y2;
                    real_t const y2 = axes.coord.port_area.y2 + axes.properties.y2.tick[1].length;
                    middle(y1, y2, axes.properties.y2.tick[1]);
                }
                {
                    real_t const y2 = axes.coord.port_area.y1;
                    real_t const y1 = axes.coord.port_area.y1 - axes.properties.y1.tick[2].length;
                    minor(y1, y2, step, axes.properties.y1.tick[2]);
                }
                {
                    real_t const y1 = axes.coord.port_area.y2;
                    real_t const y2 = axes.coord.port_area.y2 + axes.properties.y2.tick[2].length;
                    minor(y1, y2, step, axes.properties.y2.tick[2]);
                }

            }

            line_style.width(axes.properties.y1.sep.width);
            line_style.set_cap(caps::butt());
            line_style.set_join(joins::bevel());
            add_path(ras,
                assist(agge::line(axes.coord.port_area.x1, axes.coord.port_area.y1,
                                             axes.coord.port_area.x2, axes.coord.port_area.y1),
                    line_style));
            ras.sort();

            ren(surface, zero(), 0 /*no windowing*/, ras /*mask*/,
                platform_blender_solid_color(axes.properties.y1.sep.color), winding<>());

            ras.reset();
            line_style.width(axes.properties.y2.sep.width);
            line_style.set_cap(caps::butt());
            line_style.set_join(joins::bevel());
            add_path(ras,
                assist(agge::line(axes.coord.port_area.x1, axes.coord.port_area.y2,
                                             axes.coord.port_area.x2, axes.coord.port_area.y2),
                    line_style));
            ras.sort();

            ren(surface, zero(), 0 /*no windowing*/, ras /*mask*/,
                platform_blender_solid_color(axes.properties.y2.sep.color), winding<>());
            ras.reset();
        }

        void plot(platform_bitmap& surface, polyline& points, color col,
                  agge::stroke& lineStyle)
        {
            add_path(ras, assist(polyline_adaptor(points), lineStyle));
            ras.sort();

            ren(surface, zero(), 0 /*no windowing*/, ras /*mask*/,
                platform_blender_solid_color(col), winding<>());
            ras.reset();
        }

        void plot(platform_bitmap& surface, polyline& points, color col,
                  agge::dash& dashStyle,
                  agge::stroke& lineStyle)
        {
            ras.reset();
            add_path(ras,
                assist(
                    assist(polyline_adaptor(points), dashStyle),
                    lineStyle));
            ras.sort();

            ren(surface, zero(), 0 /*no windowing*/, ras /*mask*/,
                platform_blender_solid_color(col), winding<>());
        }

        virtual void draw(platform_bitmap& surface, timings&/*timings*/)
        {
            fill(surface, mkrect<int>(0, 0, surface.width(), surface.height()),
                platform_blender_solid_color(color::make(10, 10, 10)));

            drawAxesArea(surface);

            ras.set_clipping(axes.coord.port_area);

            auto wid = float(surface.width());
            auto hei = float(surface.height());
            auto scale = std::min(wid/400.0f, hei/370.0f);

            line_style.width(1.0f*scale);
            line_style.set_cap(caps::butt());
            line_style.set_join(joins::bevel());
            dash_style.remove_all_dashes();
            dash_style.add_dash(10.0f*scale, 5.0f*scale);
            dash_style.dash_start(0.5f*scale);

            plot(surface, points1, color::make(255, 0, 0, 128), dash_style, line_style);
            plot(surface, points2, color::make(0, 255, 0, 128), line_style);
        }

        virtual void resize(int width, int height)
        {
            auto const F = [](double t) { return cos(t)*exp(0.5*t); };
            axes.position = port_area_t{10.0f, 10.0f, (float)width-10.0f, (float)height-10.0f};
            axes.coord.update(port_area_t{100.0f, 40.0f, (float)width-20.0f, (float)height-100.0f});

            {
                double t = -5.;
                port_t cur;
                port_t prev = axes.coord << repr_t{t, F(t)};
                points1.clear();
                points1.move_to(prev.x, prev.y);
                for(t = -5; t < 5.; t += 0.01, prev = cur)
                {
                    cur = axes.coord << repr_t{t, F(t)};
                    points1.line_to(cur.x, cur.y);
                }
            }
            {
                points2.clear();
                points2.move_to(axes.coord << repr_t{-5., 10.});
                points2.line_to(axes.coord << repr_t{5., -10.});
            }
        }

    private:
        Axes                       axes;
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
