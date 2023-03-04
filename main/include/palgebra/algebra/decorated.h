#pragma once

#include <palgebra/algebra/rules.h>

namespace agge
{
    namespace rules
    {
        template<typename P, typename D>
        struct Decorated: PointGenerator<Decorated<P, D>>
        {
            template<typename T1, typename T2>
            Decorated(T1&& p, T2&& d)
                : points(FWD(p))
                , decorator(FWD(d))
            {}

            P points;
            D decorator;
        };

        template<typename P1, typename P2>
        Decorated<P1, P2> operator/(PointGenerator<P1>&& p1, Decorator<P2>&& p2)
        {
            return {FWD(p1)._get_(), FWD(p2)._get_()};
        }

        template<typename P1, typename P2>
        Decorated<P1 const&, P2> operator/(PointGenerator<P1> const& p1, Decorator<P2>&& p2)
        {
            return {p1._get_(), FWD(p2)._get_()};
        }

        template<typename P1, typename P2>
        Decorated<P1, P2 const&> operator/(PointGenerator<P1>&& p1, Decorator<P2> const& p2)
        {
            return {FWD(p1)._get_(), p2._get_()};
        }

        template<typename P1, typename P2>
        Decorated<P1 const&, P2 const&> operator/(PointGenerator<P1> const& p1, Decorator<P2> const& p2)
        {
            return {p1._get_(), p2._get_()};
        }
    }
}