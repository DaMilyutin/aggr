#pragma once

#include <palgebra/algebra/rules.h>
#include <palgebra/algebra/adapted.h>

namespace agge
{
    namespace rules
    {
        template<typename P1, typename P2>
        Joined<P1, P2> operator+(Yield<P1>&& p1, Yield<P2>&& p2)
        {
            return {FWD(p1)._get_(), FWD(p2)._get_()};
        }

        template<typename P1, typename P2>
        Joined<P1 const&, P2> operator+(Yield<P1> const& p1, Yield<P2>&& p2)
        {
            return {p1._get_(), FWD(p2)._get_()};
        }

        template<typename P1, typename P2>
        Joined<P1, P2 const&> operator+(Yield<P1>&& p1, Yield<P2> const& p2)
        {
            return {FWD(p1)._get_(), p2._get_()};
        }

        template<typename P1, typename P2>
        Joined<P1 const&, P2 const&> operator+(Yield<P1> const& p1, Yield<P2> const& p2)
        {
            return {p1._get_(), p2._get_()};
        }
    }
}