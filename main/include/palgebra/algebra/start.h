#pragma once

#include <palgebra/algebra/rules.h>
#include <palgebra/algebra/join.h>

namespace agge
{
    namespace rules
    {
        template<typename E>
        struct Start: Yield<Start<E>>
        {
            template<typename T1>
            Start(T1&& p)
                : under(FWD(p))
            {}
            E under;
        };

        template<typename P>
        Start<P> start(Yield<P>&& p)
        {
            return {FWD(p)._get_()};
        }

        template<typename P>
        Start<P const&> start(Yield<P> const& p)
        {
            return p._get_();
        }

        template<typename R, typename P>
        R& operator<<(Sink<R>& ras, Start<P> const& points)
        {
            R& the_ras = ras._get_();
            P const& the_points = points.under;
            auto b = the_points.begin();
            auto e = the_points.end();
            if(b == e)
                return the_ras;
            the_ras.move_to(*b);
            for(++b; b != e; ++b)
                the_ras.line_to(*b);
            return the_ras;
        }

        template<typename R, typename P1, typename P2>
        R& operator<<(Sink<R>& ras, Start<Joined<P1, P2>> const& points)
        {
            R& the_ras = ras._get_();
            return the_ras << start(points.under.y1) << points.under.y2;
        }

        template<typename R, typename P1, typename P2>
        R& operator<<(Sink<R>& ras, Start<Joined<P1, P2> const&> const& points)
        {
            R& the_ras = ras._get_();
            return the_ras << start(points.under.y1) << points.under.y2;
        }
    }
}
