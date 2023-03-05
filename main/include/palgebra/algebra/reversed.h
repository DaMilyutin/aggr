#pragma once

#include <palgebra/algebra/algebra.h>

namespace agge
{
    namespace rules
    {
        template<typename E>
        struct Reversed: PointGenerator<Reversed<E>>
        {
            template<typename T1>
            Reversed(T1&& p)
                : under(FWD(p))
            {}
            E under;
        };

        template<typename P>
        Reversed<P> reversed(PointGenerator<P>&& p)
        {
            return {FWD(p)._get_()};
        }

        template<typename P>
        Reversed<P const&> reversed(PointGenerator<P> const& p)
        {
            return p._get_();
        }

        template<typename R, typename P>
        R& operator<<(Consumer<R>& ras, Reversed<P> const& points)
        {
            R& the_ras = ras._get_();
            P const& the_points = points.under;
            auto e = the_points.begin() - 1;
            auto b = the_points.end() - 1;
            if(b == e)
                return the_ras;
            the_ras.move_to(*b);
            for(++b; b != e; --b)
                the_ras.line_to(*b);
            return the_ras;
        }
    }
}
