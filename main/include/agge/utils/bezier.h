#pragma once

#include <agge/types.h>

namespace agge
{
    class qbezier_interp
    {
    public:
        qbezier_interp(point_r b, point_r c, point_r e)
            : _b(b), _c(c), _e(e)
        {}

        point_r at(real_t t) const
        {
            const real_t _1_t = 1.0f - t;
            const real_t c[] = {_1_t * _1_t , 2.0f * t * _1_t , t * t};
            point_r ret;
            ret.x = _b.x * c[0] + _c.x * c[1] + _e.x * c[2];
            ret.y = _b.y * c[0] + _c.y * c[1] + _e.y * c[2];
            return ret;
        }

        point_r _b, _c, _e;
    };


    class cbezier_interp
    {
    public:
        cbezier_interp() = default;
        cbezier_interp(point_r b, point_r c1, point_r c2, point_r e)
            : _b(b), _c1(c1), _c2(c2), _e(e)
        {}

        point_r at(real_t t) const
        {
            const real_t _1_t = 1.0f - t;
            const real_t tm = 3.0f * t * _1_t;
            const real_t c[] = {_1_t * _1_t * _1_t, tm * _1_t, tm * t, t * t * t,};
            point_r ret;
            ret.x = _b.x * c[0] + _c1.x * c[1] + _c2.x * c[2] + _e.x * c[3];
            ret.y = _b.y * c[0] + _c1.y * c[1] + _c2.y * c[2] + _e.y * c[3];
            return ret;
        }

        point_r _b, _c1, _c2, _e;
    };
}