#pragma once

#include <agge/rendering/bitmap.h>
#include <agge/rendering/blenders.h>
#include <agge/rendering/blenders_generic.h>
#include <agge/config/config.h>
#include <agge/pixel.h>

#if defined(AGGE_PLATFORM_WINDOWS)
#include <agge/rendering/platform/bitmap.h>
namespace agge
{
    typedef agge::bitmap<agge::pixel32, agge::platform::raw_bitmap> platform_bitmap;
    typedef agge::order_bgra platform_pixel_order;
}
#elif defined(AGGE_PLATFORM_APPLE)
#include <agge/rendering/platform/bitmap.h>
namespace agge
{
    typedef agge::bitmap<agge::pixel32, agge::platform::raw_bitmap> platform_bitmap;
    typedef agge::order_argb platform_pixel_order;
}
#endif

#if defined(AGGE_ARCH_INTEL)
#include <agge/rendering/blenders_simd.h>
namespace agge
{
    typedef agge::blender_solid_color<agge::simd::blender_solid_color, platform_pixel_order> platform_blender_solid_color;
}
#else
namespace agge
{
    typedef agge::blender_solid_color_rgb<agge::pixel32, platform_pixel_order> platform_blender_solid_color;
}
#endif