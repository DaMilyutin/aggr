#pragma once

#include <palgebra/algebra/rules.h>

namespace agge
{
    namespace rules
    {
        template<typename E>
        struct Concat: PointGenerator<Concat<E>>
        {
            template<typename T1>
            Concat(T1&& p)
                : under(FWD(p))
            {}
            E under;
        };

        template<typename P>
        Concat<P> concat(PointGenerator<P>&& p)
        {
            return {FWD(p)._get_()};
        }

        template<typename P>
        Concat<P const&> concat(PointGenerator<P> const& p)
        {
            return p._get_();
        }


        template<typename R, typename P>
        R& operator<<(Consumer<R>& ras, Concat<P> const& points)
        {
            R& the_ras = ras._get_();
            P const& the_points = points.under;
            auto b = the_points.begin();
            auto e = the_points.end();
            for(; b != e; ++b)
                the_ras.line_to(*b);
            return the_ras;
        }
    }
}
