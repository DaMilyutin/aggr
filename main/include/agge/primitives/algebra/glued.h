#pragma once

#include <agge/primitives/algebra/rules.h>
#include <agge/primitives/algebra/algebra.h>


namespace agge
{
    namespace rules
    {
        template<typename E>
        struct Glued: PointGenerator<Glued<E>>
        {
            template<typename T1>
            Glued(T1&& p)
                : under(FWD(p))
            {}
            E under;
        };

        template<typename P>
        Glued<P> glued(PointGenerator<P>&& p)
        {
            return {FWD(p)._get_()};
        }

        template<typename P>
        Glued<P const&> glued(PointGenerator<P> const& p)
        {
            return p._get_();
        }

        template<typename R, typename P>
        R& operator<<(Rasterizer<R>& ras, Glued<P> const& points)
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
