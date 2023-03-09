#pragma once
#include <palgebra/algebra/rules.h>

namespace agge
{
    namespace rules
    {
        template<typename R, typename P>
        R& operator<<(Sink<R>& ras, Yield<P> const& points)
        {
            R& the_ras = ras._get_();
            P const& the_points = points._get_();
            auto b = the_points.begin();
            auto e = the_points.end();
            for(; b != e; ++b)
                the_ras.line_to(*b);
            return the_ras;
        }
    }
}