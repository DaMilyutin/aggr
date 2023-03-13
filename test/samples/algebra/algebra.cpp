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

using namespace agge;

namespace
{
    template <typename T>
    Rect<T> mkrect(T x1, T y1, T x2, T y2)
    {
        Rect<T> r = {x1, y1, x2, y2};
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
                chain1.push_back(cen + 300.f*grace::Vector_r{cosf(i), sinf(i)});
            for(int j = 0; j < 3; ++j, i += step)
                chain2.push_back(cen + 300.f*grace::Vector_r{cosf(i), sinf(i)});
            for(int j = 0; j < 4; ++j, i += step)
                chain3.push_back(cen + 300.f*grace::Vector_r{cosf(i), sinf(i)});

            chain4.assign(chain2);
            chain4 += grace::Vector_r{-400, -400};
        }

        virtual void draw(platform_bitmap& surface, timings&/*timings*/)
        {
            fill(surface, mkrect<int>(0, 0, surface.width(), surface.height()),
                platform_blender_solid_color(color::make(0, 50, 100)));
            auto wras = wrap_rasterizer(ras);


            wras.reset();
            wras << grace::cycle<1>(chain4);
            ras.sort();

            ren(surface, zero(), 0 /*no windowing*/, ras /*mask*/,
                platform_blender_solid_color(color::make(255, 0, 0)), winding<>());

            wras.reset();
            wras << grace::cycle<1>(grace::elements::Arc(grace::Point_r{500, 500}, 50.f, -agge::pi, agge::pi));
            ras.sort();

            ren(surface, zero(), 0 /*no windowing*/, ras /*mask*/,
                platform_blender_solid_color(color::make(200, 0, 255)), winding<>());


            wras.reset();
            wras << grace::cycle<1>(grace::elements::Vertex(grace::Point_r{1000, 500})
                                    + grace::elements::Arc(6, grace::Point_r{1000, 500}, 100.f, -agge::pi/2, agge::pi));
            ras.sort();

            ren(surface, zero(), 0 /*no windowing*/, ras /*mask*/,
                platform_blender_solid_color(color::make(255, 255, 0)), winding<>());


            wras.reset();
            wras << grace::cycle<1>(grace::elements::Segment(grace::Point_r{1000, 1000}, grace::Point_r{1000, 1200})
                        + grace::linspace(-0.5f, 1.f, 100)/grace::elements::Bezier<2>(grace::Point_r{900, 1000}, grace::Point_r{1000, 800}, grace::Point_r{1100, 1000}) );
            ras.sort();

            ren(surface, zero(), 0 /*no windowing*/, ras /*mask*/,
                platform_blender_solid_color(color::make(255, 0, 0)), winding<>());


            wras.reset();
            wras << grace::cycle<1>(chain1 + chain2 + chain3)
                            /grace::decorators::OrthoShift(40.f);
            ras.sort();

            ren(surface, zero(), 0 /*no windowing*/, ras /*mask*/,
                platform_blender_solid_color(color::make(155, 155, 155)), winding<>());

            ras.reset();

            //wras << closed(chain1/agge::memoize<agge::Point_r, 3>()
            //                    /agge::drop(1)
            //                    /agge::decorators::OrthoShift(40.f)  //
            //            + chain2/agge::memoize<agge::Point_r, 3>()
            //                    /agge::drop(1)
            //                    /agge::decorators::OrthoShift(40.f)
            //            + chain3/agge::memoize<agge::Point_r, 3>()
            //                    /agge::drop(1)
            //                    /agge::decorators::OrthoShift(40.f));
            //ras.sort();

            //ren(surface, zero(), 0 /*no windowing*/, ras /*mask*/,
            //    platform_blender_solid_color(color::make(255, 255, 255)), winding<>());

            wras.reset();
            wras << grace::cycle<1>(chain1 + chain2 + chain3);
            ras.sort();

            ren(surface, zero(), 0 /*no windowing*/, ras /*mask*/,
                platform_blender_solid_color(color::make(0, 255, 0)), winding<>());

        }

    private:
        rasterizer< clipper<int> > ras;
        renderer ren;

        Chain chain1;
        Chain chain2;
        Chain chain3;
        Chain chain4;
    };
}

application* agge_create_application(services&/*s*/)
{
    return new Algebra;
}
