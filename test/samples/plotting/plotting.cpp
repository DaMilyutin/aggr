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
#include <samples/common/timing.h>

#include <algorithm>
#include <chrono>
#include <optional>

#include <plotting/generators/transform_or.h>

namespace
{
    template <typename T>
    agge::rect<T> mkrect(T x1, T y1, T x2, T y2)
    {
        return {x1, y1, x2, y2};
    }

    inline bool in_area(plotting::repr_area_t const& a, plotting::repr_t const& p)
    {
        return (a.x1 < p.x && p.x < a.x2) && (a.y2 < p.y && p.y < a.y1);
    }

    inline bool in_area(agge::rect_r const& a, agge::point_r const& p)
    {
        return (a.x1 < p.x && p.x < a.x2) && (a.y1 < p.y && p.y < a.y2);
    }

    class Plotting: public application
    {
    public:
        Plotting()
            : application()
        {
            axes.coordinates.repr_area = mkrect(-5., 2.0, 5., -2.0);

            //axes.coordinates.repr_area = mkrect(-5.e-5, 2.e-5, 5.e-5, -2.e-5);
            origin = axes.coordinates.repr_area;
            line_style.set_cap(agge::caps::butt());
            line_style.set_join(agge::joins::bevel());
            line_style.width(2.0f);
            dash_style.remove_all_dashes();
            dash_style.add_dash(10.0f, 1.0f);
            dash_style.dash_start(0.5f);

            axes.properties.y1.tickSteps = 5;
            axes.properties.y1.tickGap = 10;

            axes.properties.y2.tickSteps = 5;
            axes.properties.y2.tickGap = 10;

            auto t = std::chrono::system_clock::now();
            std::chrono::time_point days = std::chrono::round<std::chrono::days>(t);
            //axes.properties.x2.tickSteps = 20;
            //axes.properties.x2.tickGap = 120;
            axes.properties.x2.labels.base.vertical(agge::align_far);
            axes.properties.x2.labels.format = [days, p = std::chrono::days(std::numeric_limits<std::chrono::days::rep>::min())](double x) mutable
            {
                std::chrono::time_point t = days + std::chrono::round<std::chrono::seconds>(std::chrono::duration<double>(x*86400.*0.125));
                auto c = std::chrono::floor<std::chrono::days>(t.time_since_epoch());
                std::string ret;
                constexpr auto undef = std::chrono::days(std::numeric_limits<std::chrono::days::rep>::min());
                if(c == p /*|| p == undef*/)
                    ret = std::format("{:%T}", t);
                else
                    ret = std::format("{:%T\n%F}", t);
                p = c;
                return ret;
            };

            axes.properties.x2.grid.color = agge::color{255, 255, 0, 128};


            //auto const F = [](double t) { return sin(t/(t*t+1.e-2))*exp(0.5*t); };
            //chart.from(plotting::range(-5., 5., 1.e-2)/plotting::FunctionX(F));

            chart << plotting::range(0., 50., 1.e-4)
                        /plotting::FunctionXY([](double t){return 0.01*t*sin(t)+3.; },
                                              [](double t){return 0.01*t*cos(t)+1; });
        }
    private:

        virtual void draw(agge::platform_bitmap& surface, timings& timings)
        {
            stopwatch(_counter);
            fill(surface, mkrect<int>(0, 0, surface.width(), surface.height()),
                 agge::platform_blender_solid_color(agge::color::make(10, 10, 10)));
            timings.clearing = stopwatch(_counter);

            auto canvas = plotting::make_canvas(surface, ren, ras);

            canvas << axes;

            canvas.ras.set_clipping(axes.coordinates.port_area);

            canvas << plotting::reset
                << agge::polyline_adaptor(points1)/dash_style/line_style
                << agge::color::make(255, 0, 0, 128);

            canvas << plotting::reset
                << agge::polyline_adaptor(points2)/line_style
                << agge::color::make(0, 255, 0, 128);
            timings.rendition = stopwatch(_counter);
            timings.stroking = _timingOfUpdate;
        }

        void update_data()
        {
            stopwatch(_counter);
            points1 << agge::clear;


            points1 << chart/plotting::transform_or([&, cmd = unsigned(agge::path_command_line_to), cmd_next = unsigned(agge::path_command_line_to)]
                                                        (plotting::repr_t const& x) mutable
                                                   {  bool const b = in_area(axes.coordinates.repr_area, x);
                                                      if(!b) { cmd = cmd_next = agge::path_command_move_to; return std::optional<agge::polyline::Item>(); }
                                                      cmd = cmd_next;
                                                      cmd_next = agge::path_command_line_to;
                                                      auto n = axes.coordinates.repr2port(x);
                                                      return std::make_optional(agge::polyline::Item{n, cmd}); })
                                                    /plotting::filters::FarEnough{0.5f};



            //unsigned cmd      = agge::path_command_move_to;
            //points1 << chart/plotting::filter([&, cmd_next = unsigned(agge::path_command_line_to) ] (plotting::repr_t const& x) mutable
            //                                    {  bool const b = in_area(axes.coordinates.repr_area, x);
            //                                       if(!b) cmd_next = agge::path_command_move_to;
            //                                       cmd = cmd_next;
            //                                       if(b) cmd_next = agge::path_command_line_to;
            //                                       return b; })
            //                /axes.coordinates.repr2port
            //                /plotting::filters::FarEnough{0.5f}
            //                /plotting::transform([&](plotting::port_t const& x)
            //                                     {  return agge::polyline::Item{x, cmd}; });


            //unsigned cmd = agge::path_command_move_to;
            //plotting::filters::FarEnough farenough{0.5f};
            //for(auto&& p: chart)
            //{
            //    if(!in_area(axes.coordinates.repr_area, p))
            //    {
            //        cmd = agge::path_command_move_to;
            //        continue;
            //    }
            //    auto n = axes.coordinates.repr2port(p);
            //    if(!farenough(n))
            //        continue;
            //    points1.push_back({n, cmd});
            //    cmd = agge::path_command_line_to;
            //}

            //points1 << chart/plotting::filter([&](plotting::repr_t const& x)
            //                                    { return in_area(axes.coordinates.repr_area, x); })
            //                /axes.coordinates.repr2port
            //                /plotting::filters::FarEnough{0.5f};


            //points2 << agge::clear
            //        << chart/plotting::transform([](plotting::repr_t p) { return plotting::repr_t{-p.x, -p.y}; })
            //            /axes.coordinates.repr2port
            //            /plotting::filters::FarEnough{{},50.0f};
            _timingOfUpdate = stopwatch(_counter);
        }

        virtual void resize(int width, int height)
        {
            axes.position = plotting::port_area_t{10.0f, 10.0f, (float)width-10.0f, (float)height-10.0f};
            axes.coordinates.update(plotting::port_area_t
                {100.0f, 40.0f,
                 std::max((float)width-120.0f, 105.0f),
                 std::max((float)height-100.0f, 45.0f)});

            update_data();
        }

        virtual void consume_events() override
        {
            auto const input = events.read();

            if(input.vWheel != 0)
            {
                agge::point_r ref_point{(float)input.position.x, (float)input.position.y};
                if(in_area(axes.coordinates.port_area, ref_point))
                {
                    auto n = axes.coordinates.repr_area;
                    auto orig = axes.coordinates.port2repr(ref_point);
                    plotting::zoom(n, orig, exp(-log(1.5)*input.vWheel/120.));
                    axes.coordinates.update(n);
                }
            }
            if(input.mouse.pressed[system_input::MouseButtons::Left])
            {
                agge::point_r ref_point{(float)input.position.x, (float)input.position.y};
                if(in_area(axes.coordinates.port_area, ref_point))
                {
                    agge::vector_r shift_repr{-(float)input.move.x, -(float)input.move.y};
                    auto n = axes.coordinates.repr_area;
                    auto shift = axes.coordinates.port2repr(shift_repr);
                    plotting::shift(n, shift);
                    axes.coordinates.update(n);
                }
            }

            //plotting::Text hello(agge::font_descriptor::create("Times New Roman", 25,
            //    agge::regular, false, agge::hint_none));
            //hello.position({agge::real_t(input.position.x), agge::real_t(input.position.y)});
            //hello.text(std::format("({},{})", input.position.x, input.position.y));
            //hello.color(input.mouse.pressed[system_input::MouseButtons::Left]
            //    ? agge::color{255, 0, 0, 255} : agge::color{128, 128, 128, 128});

            update_data();
        }

    private:
        plotting::Axes             axes;
        plotting::repr_area_t      origin;
        plotting::rasterizer       ras;
        agge::renderer             ren;
        agge::stroke               line_style;
        agge::dash                 dash_style;
        agge::polyline             points1;
        agge::polyline             points2;

        plotting::ChartData        chart;
        float                      scale = 1.0f;

        long long                   _counter = 0;
        double                      _timingOfUpdate = 0.;
    };
}

application* agge_create_application(services&/*s*/)
{
    return new Plotting;
}
