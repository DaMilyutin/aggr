#pragma once

#include <palgebra/algebra/rules.h>
#include <palgebra/algebra/join.h>
#include <palgebra/algebra/concat.h>
#include <palgebra/algebra/closed.h>

namespace agge
{
    namespace rules
    {
        template<typename R, typename P>
        R& operator<<(Rasterizer<R>& ras, PointGenerator<P> const& points)
        {
            R& the_ras = ras._get_();
            P const& the_points = points._get_();
            auto b = the_points.begin();
            auto e = the_points.end();
            if(b == e)
                return the_ras;
            the_ras.move_to(*b);
            for(++b; b != e; ++b)
                the_ras.line_to(*b);
            return the_ras;
        }
    }
}