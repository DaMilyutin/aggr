#pragma once

#include <palgebra/algebra/algebra.h>

namespace agge
{
    namespace rules
    {
        template<typename E>
        struct Closed: PointGenerator<Closed<E>>
        {
            template<typename T1>
            Closed(T1&& p)
                : under(FWD(p))
            {}
            E under;
        };

        template<typename P>
        Closed<P> closed(PointGenerator<P>&& p)
        {
            return {FWD(p)._get_()};
        }

        template<typename P>
        Closed<P const&> closed(PointGenerator<P> const& p)
        {
            return p._get_();
        }

        template<typename R, typename P>
        R& operator<<(Rasterizer<R>& ras, Closed<P> const& points)
        {
            R& the_ras = ras._get_();
            the_ras << points.under;
            the_ras.close_polygon();
            return the_ras;
        }
    }
}
