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

        MarkerPolygonMaker(MarkerPolygonProperties const& b): inner(b) {}
        MarkerPolygonMaker(MarkerPolygonProperties&& b): inner(std::move(b)) {}

        MarkerPolygon mutable inner;

        MarkerPolygon const& operator()(agge::point_r const& position) const
        {
            inner.position(position);
            return inner;
        }
    };
}