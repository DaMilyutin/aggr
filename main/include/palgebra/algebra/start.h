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

            template<typename R>
            bool send_to(Sink<R>& ras) const
            {
                R& the_ras = ras._get_();
                E const& the_points = under;
                auto b = the_points.begin();
                auto e = the_points.end();
                if(b == e)
                    return false;
                the_ras.move_to(*b);
                for(++b; b != e; ++b)
                    the_ras.line_to(*b);
                return true;
            }

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
            points.send_to(ras);
            return ras._get_();
        }

        template<typename R, typename P1, typename P2>
        R& operator<<(Sink<R>& ras, Start<Joined<P1, P2> const&> const& points)
        {
            if(start(points.under.y1).send_to(ras))     // if we started on y1
                return ras << points.under.y2;          //   we just continue on y2
            else                                        // if we couldn't start on y1
                return ras << start(points.under.y2);   //   we just try to start on y2
        }

        template<typename R, typename P1, typename P2>
        R& operator<<(Sink<R>& ras, Start<Joined<P1, P2>> const& points)
        {
            R& the_ras = ras._get_();
            auto const& the_points = points.under;
            return the_ras << start(the_points);
        }


    }
}
