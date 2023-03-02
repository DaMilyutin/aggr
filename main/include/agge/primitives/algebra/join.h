#pragma once

#include <agge/primitives/algebra/rules.h>

namespace agge
{
    namespace rules
    {
        template<typename P1, typename P2>
        struct Joined: PointGenerator<Joined<P1, P2>, false>
        {
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
        R& operator<<(Rasterizer<R>& ras, Joined<P1, P2> const& points)
        {
            R& the_ras = ras._get_();
            return the_ras << points.p1 << points.p2;
        }

    }
}