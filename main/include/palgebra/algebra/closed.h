#pragma once

#include <palgebra/algebra/algebra.h>
#include <palgebra/algebra/start.h>
#include <palgebra/elements/Vertex.h>

namespace agge
{
    namespace rules
    {
        template<typename E>
        struct Closed: Yield<Closed<E>>
        {
            template<typename T1>
            Closed(T1&& p)
                : under(FWD(p))
            {}
            E under;

            // stub
            auto begin() const { return std::begin(under); }
            auto end()   const { return std::end(under); }
        };

        template<typename P>
        Closed<P> closed(Yield<P>&& p)
        {
            return {FWD(p)._get_()};
        }

        template<typename P>
        Closed<P const&> closed(Yield<P> const& p)
        {
            return {p._get_()};
        }

        template<typename P, typename S>
        bool transfuse(Closed<P> const& points, S& the_ras)
        {
            std::optional<Point_r> s = starting(points.under, the_ras);
            if(s) the_ras.consume(*s);
            return true;
        }
    }
}
