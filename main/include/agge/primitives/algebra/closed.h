#pragma once

#include <agge/primitives/algebra/rules.h>
#include <agge/primitives/algebra/algebra.h>


namespace agge
{
    namespace rules
    {
        template<typename E>
        struct Enclosed: PointGenerator<Enclosed<E>>
        {
            template<typename T1>
            Enclosed(T1&& p)
                : under(FWD(p))
            {}
            E under;
        };

        template<typename P>
        Enclosed<P> closed(PointGenerator<P>&& p)
        {
            return {FWD(p)._get_()};
        }

        template<typename P>
        Enclosed<P const&> closed(PointGenerator<P> const& p)
        {
            return p._get_();
        }

        template<typename R, typename P>
        R& operator<<(Rasterizer<R>& ras, Enclosed<P> const& points)
        {
            R& the_ras = ras._get_();
            the_ras << points.under;
            the_ras.close_polygon();
            return the_ras;
        }
    }
}