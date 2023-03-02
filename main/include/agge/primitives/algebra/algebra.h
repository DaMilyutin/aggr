#pragma once

#include <agge/primitives/algebra/rules.h>

namespace agge
{
    namespace rules
    {
        template<typename R, typename P, bool closed>
        R& operator<<(Rasterizer<R>& ras, PointGenerator<P, closed> const& points)
        {
            R& the_ras = ras._get_();
            P& the_points = points._get_();
            auto b = points.begin();
            auto e = points.end();
            if(b == e)
                return the_ras;
            the_ras.move_to(b->x, b->y);
            for(++b; b != e; ++b)
                the_ras.line_to(b->x, b->y);
            if(closed)
                the_ras.close_polygon();
            return the_ras;
        }
    }
}