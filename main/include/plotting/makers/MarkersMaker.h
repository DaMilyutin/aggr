#pragma once

#include <plotting/piping.h>
#include <plotting/primitives/Marker.h>

namespace plotting
{
    struct MarkerPolygonMaker: piping::Transform<MarkerPolygonMaker>
    {
        MarkerPolygonMaker() = default;
        MarkerPolygonMaker(MarkerPolygonMaker&&) = default;
        MarkerPolygonMaker(MarkerPolygonMaker const&) = default;

        template<typename B>
        MarkerPolygonMaker(B&& b): inner(FWD(b)) {}

        MarkerPolygon mutable inner;

        MarkerPolygon const& operator()(agge::point_r const& position) const
        {
            inner.position(position);
            return inner;
        }
    };
}