#pragma once

#include <agge/types/Point.h>

namespace agge
{
    template <typename T>
    struct Rect
    {
        Point<T> min;
        Point<T> max;

        T width() const
        {
            return max.x - min.x;
        }

        T height() const
        {
            return max.y - min.y;
        }

        void widen_x(Point<T> const& p)
        {
            if(p.x < min.x) min.x = p.x;
            else if(p.x > max.x) max.x = p.x;
        }

        void widen_y(Point<T> const& p)
        {
            if(p.y < min.y) min.y = p.y;
            else if(p.y > max.y) max.y = p.y;
        }

        void widen(Point<T> const& p)
        {
            widen_x(p);
            widen_y(p);
        }

        bool has(Point<T> const& p) const
        {
            return (min.x < p.x && p.x < max.x) && (min.y < p.y && p.y < max.y);
        }
    };

    template <typename T>
    struct Box
    {
        T w, h;
    };

    typedef Rect<int> rect_i;
    typedef Rect<real_t> rect_r;
    typedef Box<real_t> box_r;

    template <typename CoordT>
    inline const Rect<CoordT>& operator +=(Rect<CoordT>& lhs, const Vector<CoordT>& rhs)
    {
        return lhs.min += rhs, lhs.max += rhs, lhs;
    }

    template <typename CoordT>
    inline Rect<CoordT> create_rect(CoordT x1, CoordT y1, CoordT x2, CoordT y2)
    {
        Rect<CoordT> r = {x1, y1, x2, y2};
        return r;
    }

    template <typename CoordT>
    inline Box<CoordT> create_box(CoordT w, CoordT h)
    {
        Box<CoordT> b = {w, h};
        return b;
    }

    inline rect_i create_rect_i(agge::rect_r const& r)
    {
        return agge::rect_i{int(round(r.min.x)), int(round(r.min.y)),
                            int(round(r.max.x)), int(round(r.max.y))};
    };

}