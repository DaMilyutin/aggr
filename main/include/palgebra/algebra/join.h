#pragma once

#include <palgebra/algebra/rules.h>

namespace agge
{
    namespace rules
    {
        template<typename P1, typename P2>
        struct Joined: PointGenerator<Joined<P1, P2>>
        {
            template<typename T1, typename T2>
            Joined(T1&& l1, T2&& l2)
                : p1(FWD(l1))
                , p2(FWD(l2))
            {}

            P1 p1;
            P2 p2;
        };

        template<typename P1, typename P2>
        Joined<P1, P2> operator+(PointGenerator<P1>&& p1, PointGenerator<P2>&& p2)
        {
            return {FWD(p1)._get_(), FWD(p2)._get_()};
        }

        template<typename P1, typename P2>
        Joined<P1 const&, P2> operator+(PointGenerator<P1> const& p1, PointGenerator<P2>&& p2)
        {
            return {p1._get_(), FWD(p2)._get_()};
        }

        template<typename P1, typename P2>
        Joined<P1, P2 const&> operator+(PointGenerator<P1>&& p1, PointGenerator<P2> const& p2)
        {
            return {FWD(p1)._get_(), p2._get_()};
        }

        template<typename P1, typename P2>
        Joined<P1 const&, P2 const&> operator+(PointGenerator<P1> const& p1, PointGenerator<P2> const& p2)
        {
            return {p1._get_(), p2._get_()};
        }

        template<typename R, typename P1, typename P2>
        R& operator<<(Consumer<R>& ras, Joined<P1, P2> const& points)
        {
            R& the_ras = ras._get_();
            return the_ras << points.p1 << concat(points.p2);
        }
    }
}