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

        Start<Point_r const&> start(Point_r const& p)
        {
            return {p};
        }

        template<typename E, typename R>
        bool starting(E const& the_points, Sink<R>& ras)
        {
            R& the_ras = ras._get_();
            auto b = the_points.begin();
            auto e = the_points.end();
            if(b == e)
                return false;
            the_ras << start(*b);
            for(++b; b != e; ++b)
                the_ras << *b;
            return true;
        }

        template<typename R, typename P1, typename P2>
        bool starting(Joined<P1, P2> const& points, Sink<R>& ras)
        {
            if(!starting(points.y1, ras))        // if we couldn't start on y1
                return starting(points.y2, ras); //   we just try to start on y2
            ras << points.y2; // if we started on y1 we just continue on y2
            return true;
        }

        template<typename R, typename P>
        R& operator<<(Sink<R>& ras, Start<P> const& points)
        {
            starting(points.under, ras);
            return ras._get_();
        }

        template<typename R>
        R& operator<<(Sink<R>& ras, Start<Point_r const&> const& p)
        {
            R& the_ras = ras._get_();
            the_ras.move_to(p.under);
            return ras._get_();
        }
    }
}
