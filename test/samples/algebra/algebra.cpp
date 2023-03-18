#include <agge/rendering/clipper.h>
#include <agge/rendering/filling_rules.h>
#include <agge/rendering/renderer.h>
#include <agge/rendering/rasterizer.h>

#include <samples/common/shell.h>

#include <grace/algebra.h>
#include <grace/elements.h>
#include <grace/decorators.h>


#define _USE_MATH_DEFINES
#include <math.h>

namespace
{
    template <typename T>
    agge::Rect<T> mkrect(T x1, T y1, T x2, T y2)
    {
        agge::Rect<T> r = {x1, y1, x2, y2};
        return r;
    }

    struct Chain: grace::rules::Yield<Chain>, agge::pod_vector<grace::Point_r>
    {
        using agge::pod_vector<grace::Point_r>::begin;
        using agge::pod_vector<grace::Point_r>::end;
    };

    Chain& operator+=(Chain& c, grace::Vector_r const& s)
    {
        for(auto& p: c)
            p += s;
        return c;
    }

    Chain& operator-=(Chain& c, grace::Vector_r const& s)
    {
        for(auto& p: c)
            p -= s;
        return c;
    }

    class Algebra: public application
    {
    public:
        Algebra()
        {
            grace::Point_r const cen = {800, 800};

            int const N = 9;
            int const k = 4;

            float const step = float(k*2*M_PI/N);
            grace::real_t i = float(-M_PI/2);
            for(int j = 0; j < 2; ++j, i += step)
                chain1.push_back(cen + grace::Vector_r::polar(300.f,i));
            for(int j = 0; j < 3; ++j, i += step)
                chain2.push_back(cen + grace::Vector_r::polar(300.f, i));
            for(int j = 0; j < 4; ++j, i += step)
                chain3.push_back(cen + grace::Vector_r::polar(300.f, i));

            chain4.assign(chain2);
            chain4 += grace::Vector_r{-400, -400};

            push_back(chain5) << (chain1 + chain2 + chain3);

            chain5 += grace::Vector_r{400, 0};

        }

        void render_color(agge::platform_bitmap& surface, agge::color color)
        {
            ras.sort();
            ren(surface, agge::zero(), 0 /*no windowing*/, ras /*mask*/,
                agge::platform_blender_solid_color(color), agge::winding<>());
        }

        virtual void draw(agge::platform_bitmap& surface, timings&/*timings*/)
        {
            fill(surface, mkrect<int>(0, 0, surface.width(), surface.height()),
                agge::platform_blender_solid_color(agge::color::make(0, 50, 100)));
            auto wras = agge::wrap_rasterizer(ras);


            wras.reset();
            wras << cycle<1>(chain4);
            ras.sort();
            render_color(surface, agge::color::make(255, 200, 100));


            wras.reset();
            wras << cycle<1>(grace::elements::Arc(grace::Point_r{500, 500}, 50.f, -agge::pi, agge::pi));
            render_color(surface, agge::color::make(200, 0, 255));

            wras.reset();
            wras << cycle<1>(grace::elements::Vertex(grace::Point_r{1000, 500})
                                    + grace::elements::Arc(6, grace::Point_r{1000, 500}, 100.f, -agge::pi/2, agge::pi));
            render_color(surface, agge::color::make(255, 255, 0));


            wras.reset();
            wras << cycle<1>(grace::elements::Segment(grace::Point_r{1000, 1000}, grace::Point_r{1000, 1200})
                        + grace::linspace(-0.5f, 1.f, 100)/grace::elements::Bezier<2>(grace::Point_r{900, 1000}, grace::Point_r{1000, 800}, grace::Point_r{1100, 1000}) );
            render_color(surface, agge::color::make(255, 0, 0));


            wras.reset();
            wras << cycle<1>(chain1 + chain2 + chain3)
                            /grace::memoize<grace::Point_r, 3>()
                            /grace::decorators::OrthoShift(40.f);
            render_color(surface, agge::color::make(155, 155, 155));

            wras.reset();
            wras << grace::cycle<1>(chain1 + chain2 + chain3);
            render_color(surface, agge::color::make(0, 255, 0));


            wras.reset();
            wras << cycle<1>(chain1 + chain2 + chain3)
                /grace::memoize<grace::Point_r, 3>()
                /grace::decorators::OrthoShift(40.f);
            render_color(surface, agge::color::make(155, 155, 155));

            wras.reset();
            wras << grace::cycle<1>(chain1 + chain2 + chain3);
            render_color(surface, agge::color::make(0, 255, 0));

            wras.reset();
            [&]()
            {
                auto rng = as_range(grace::elements::Arc(grace::Point_r{1500, 1000}, 300.f, -agge::pi, agge::pi));
                grace::elements::Keeper<void> keeper;
                grace::elements::Skipper skipper;
                grace::real_t const length_limit = 100;
                grace::real_t const gap_limit = 300;

                grace::Point_r last;
                keeper.length_limit = length_limit;
                keeper.clear();
                while(rng.iterator != rng.sentinel)
                {
                    rng = rng/keeper;
                    if(keeper.path.size() < 2)
                        return;

                    last = keeper.path.back();
                    keeper.path.back() = towards(last, keeper.path[keeper.path.size()-2], keeper.length_limit);

                    wras << grace::as_range(keeper.path);

                    skipper.length_limit = gap_limit;
                    if(skipper.start(keeper.path.back(), last))
                        rng = rng/skipper;

                    if(rng.iterator == rng.sentinel)
                        return;
                    keeper.length_limit = length_limit;
                    keeper.clear();
                    keeper.consume(towards(skipper.prev, skipper.last, skipper.length_limit));
                }
            }();
            wras.close_polygon();
            render_color(surface, agge::color::make(0, 255, 255));
        }

    private:
        agge::rasterizer<agge::clipper<int>> ras;
        agge::renderer                       ren;

        Chain chain1;
        Chain chain2;
        Chain chain3;
        Chain chain4;
        Chain chain5;
    };
}

application* agge_create_application(services&/*s*/)
{
    return new Algebra;
}
