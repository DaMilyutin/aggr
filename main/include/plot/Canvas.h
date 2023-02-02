#pragma once
#include <agge/rendering/blenders_generic.h>
#include <agge/utils/tools.h>
#include <agge/rendering/filling_rules.h>

namespace plotting
{
    template<typename SurfaceT, typename RendererT, typename RasterizerT>
    struct Canvas
    {
    public:
        SurfaceT*   surface;
        RendererT   ren;
        RasterizerT ras;

        template<typename PixelT, typename OrderT>
        Canvas& operator<<(
            agge::blender_solid_color_rgb<PixelT, OrderT>& bl)
        {
            ras.sort();
            ren(*surface, agge::zero(), 0 /*no windowing*/, ras /*mask*/,
                bl, agge::winding<>());
        }
    };


}