#pragma once

#include <agge/rendering/rasterizer.h>
#include <agge/rendering/clipper.h>

namespace plotting
{
    using rasterizer = agge::rasterizer< agge::clipper<int> >;
}