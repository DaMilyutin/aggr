#include <agge/rendering/clipper.h>
#include <agge/rendering/filling_rules.h>
#include <agge/rendering/renderer.h>
#include <agge/rendering/rasterizer.h>

#include <samples/common/shell.h>

#include <agge/primitives/algebra/algebra.h>
#include <agge/primitives/algebra/join.h>
#include <agge/primitives/algebra/closed.h>

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

    struct Chain: agge::rules::PointGenerator<Chain>, agge::pod_vector<Point_r>
    {};

    class Algebra: public application
    {
    public:
        Algebra()
        {
            agge::Point_r const cen = {500, 500};

            int const N = 9;
            int const k = 4;

            double const step = k*2*M_PI/N;
            agge::real_t i = -M_PI/2;
            for(int j = 0; j < 2; ++j, i += step)
                chain1.push_back(cen + 200.f*agge::Vector_r{cosf(i), sinf(i)});
            for(int j = 0; j < 3; ++j, i += step)
                chain2.push_back(cen + 250.f*agge::Vector_r{cosf(i), sinf(i)});
            for(int j = 0; j < 4; ++j, i += step)
                chain3.push_back(cen + 300.f*agge::Vector_r{cosf(i), sinf(i)});
        }

        virtual void draw(platform_bitmap& surface, timings&/*timings*/)
        {
            fill(surface, mkrect<int>(0, 0, surface.width(), surface.height()),
                platform_blender_solid_color(color::make(0, 50, 100)));

            ras.reset();
            ras << closed(chain1 + chain2 + chain3);
            ras.sort();

            ren(surface, zero(), 0 /*no windowing*/, ras /*mask*/,
                platform_blender_solid_color(color::make(255, 255, 255)), winding<>());
        }

    private:
        rasterizer< clipper<int> > ras;
        renderer ren;

        Chain chain1;
        Chain chain2;
        Chain chain3;
    };
}

application* agge_create_application(services&/*s*/)
{
    return new Algebra;
}