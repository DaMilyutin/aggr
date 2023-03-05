#pragma once

#include "algebra/algebra.h"
#include "algebra/rules.h"
#include "algebra/start.h"
#include "algebra/join.h"
#include "algebra/closed.h"
#include "algebra/decorated.h"

namespace agge
{
    namespace rules
    {
        template<typename P1, typename P2, typename D>
        auto operator/(Joined<P1, P2>&& j, Decorator<D>&& d)
        {
            return FWD(FWD(j).p1)/FWD(d)._get_() + FWD(FWD(j).p2)/FWD(d)._get_();
        }

        template<typename P1, typename P2, typename D>
        auto operator/(Joined<P1, P2> const& j, Decorator<D>&& d)
        {
            return j.p1/FWD(d)._get_() + j.p2/FWD(d)._get_();
        }

        template<typename P1, typename P2, typename D>
        auto operator/(Joined<P1, P2>&& j, Decorator<D> const& d)
        {
            return FWD(FWD(j).p1)/d._get_() + FWD(FWD(j).p2)/d._get_();
        }

        template<typename P1, typename P2, typename D>
        auto operator/(Joined<P1, P2> const& j, Decorator<D> const& d)
        {
            return j.p1/d._get_() + j.p2/d._get_();
        }
    }
}