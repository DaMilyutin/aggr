#include <agge/rendering/clipper.h>
#include <agge/rendering/filling_rules.h>
#include <agge/rendering/renderer.h>
#include <agge/rendering/rasterizer.h>

#include <samples/common/shell.h>

using namespace agge;

namespace
{
    template <typename T>
    Rect<T> mkrect(T x1, T y1, T x2, T y2)
    {
        Rect<T> r = {x1, y1, x2, y2};
        return r;
    }

    class RasterizerApp: public application
    {
    private:
        virtual void draw(platform_bitmap& surface, timings&/*timings*/)
        {
            fill(surface, mkrect<int>(0, 0, surface.width(), surface.height()),
                platform_blender_solid_color(color::make(0, 50, 100)));

            ras.reset();

            ras.move_to(10.0f, 50.0f);
            ras.line_to(190.0f, 60.0f);
            ras.line_to(20.0f, 130.0f);
            ras.close_polygon();

            ras.sort();

            ren(surface, zero(), 0 /*no windowing*/, ras /*mask*/,
                platform_blender_solid_color(color::make(255, 255, 255)), winding<>());

            ras.reset();
            ras.move_to(10.0f, 50.0f + 200.0f);
            ras.line_to(20.0f, 130.0f + 200.0f);
            ras.line_to(190.0f, 60.0f + 200.0f);
            ras.close_polygon();
            ras.sort();

            ren(surface, zero(), 0 /*no windowing*/, ras /*mask*/,
                platform_blender_solid_color(color::make(255, 0, 0)), winding<>());

            ras.reset();
            ras.move_to(10.0f  + 400.0f, 50.0f + 200.0f);
            ras.line_to(20.0f  + 400.0f, 130.0f + 200.0f);
            ras.line_to(190.0f + 400.0f, 60.0f + 200.0f);
            ras.sort();

            ren(surface, zero(), 0 /*no windowing*/, ras /*mask*/,
                platform_blender_solid_color(color::make(0, 255, 0)), winding<>());

            ras.reset();
            ras.move_to(10.0f  + 400.0f, 50.0f);
            ras.line_to(190.0f + 400.0f, 60.0f);
            ras.line_to(20.0f  + 400.0f, 130.0f);
            ras.sort();

            ren(surface, zero(), 0 /*no windowing*/, ras /*mask*/,
                platform_blender_solid_color(color::make(255, 255, 0)), winding<>());

        }

    private:
        rasterizer< clipper<int> > ras;
        renderer ren;
    };
}

application* agge_create_application(services&/*s*/)
{
    return new RasterizerApp;
}
