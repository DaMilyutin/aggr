#pragma once
#include <palgebra/algebra/rules.h>
#include <agge/types/point.h>
#include <agge/math/bezier.h>

namespace agge
{
    namespace elements
    {
        // point generators for
        //  intermediary points
        template<size_t N, typename P = Point_r>
        class Bezier;


        template<typename P>
        class Bezier<2, P>: public rules::Transform<Bezier<2, P>>
        {
        public:
            Bezier() = default;
            Bezier(P b, P c, P e)
                : _b(b), _c(c), _e(e)
            {}

            P operator()(real_t t) const
            {
                const real_t _1_t = 1.0f - t;
                const real_t c[] = {_1_t * _1_t , 2.0f * t * _1_t , t * t};
                P ret;
                ret.x = _b.x * c[0] + _c.x * c[1] + _e.x * c[2];
                ret.y = _b.y * c[0] + _c.y * c[1] + _e.y * c[2];
                return ret;
            }

            P _b, _c, _e;
        };


        template<typename P>
        class Bezier<3, P>: public rules::Transform<Bezier<3, P>>
        {
        public:
            Bezier() = default;
            Bezier(P b, P c1, P c2, P e)
                : _b(b), _c1(c1), _c2(c2), _e(e)
            {}

            P operator()(real_t t) const
            {
                const real_t _1_t = 1.0f - t;
                const real_t tm = 3.0f * t * _1_t;
                const real_t c[] = {_1_t * _1_t * _1_t, tm * _1_t, tm * t, t * t * t,};
                P ret;
                ret.x = _b.x * c[0] + _c1.x * c[1] + _c2.x * c[2] + _e.x * c[3];
                ret.y = _b.y * c[0] + _c1.y * c[1] + _c2.y * c[2] + _e.y * c[3];
                return ret;
            }

            P _b, _c1, _c2, _e;
        };

    }
}