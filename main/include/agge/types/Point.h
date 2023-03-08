#pragma once
#include <agge/types/scalars.h>

#include <math.h>

namespace agge
{
    template <typename T>
    struct Vector
    {
        T x;
        T y;
    };

    template <typename T>
    struct Point
    {
        T x;
        T y;
    };

    template<typename PV>
    struct coord_type;

    template<typename T>
    struct coord_type<Point<T>>
    {
        typedef T type;
    };

    template<typename T>
    struct coord_type<Vector<T>>
    {
        typedef T type;
    };

    template<typename PV>
    using coord_type_t = typename coord_type<PV>::type;

    template<typename P, typename X>
    constexpr auto as_coord_cast(X x) { return static_cast<coord_type_t<P>>(x); }

    typedef Point<real_t>  Point_r;
    typedef Vector<int>    Vector_i;
    typedef Vector<real_t> Vector_r;

    template <typename CoordT>
    inline Vector<CoordT> operator -(const Point<CoordT>& lhs, const Point<CoordT>& rhs)
    {
        const Vector<CoordT> result = {lhs.x - rhs.x, lhs.y - rhs.y};
        return result;
    }

    template <typename CoordT>
    inline Vector<CoordT> operator *(CoordT lhs, const Vector<CoordT>& rhs)
    {
        const Vector<CoordT> result = {lhs * rhs.x, lhs * rhs.y};
        return result;
    }

    template <typename CoordT>
    inline Vector<CoordT> operator *(const Vector<CoordT>& lhs, CoordT rhs)
    {
        const Vector<CoordT> result = {rhs * lhs.x, rhs * lhs.y};
        return result;
    }

    template <typename CoordT>
    inline Vector<CoordT>& operator*=(Vector<CoordT>& lhs, CoordT rhs)
    {
        lhs.x *= rhs; lhs.y *= rhs;
        return lhs;
    }

    template <typename CoordT>
    inline const Vector<CoordT>& operator +=(Vector<CoordT>& lhs, const Vector<CoordT>& rhs)
    {
        return lhs.x += rhs.x, lhs.y += rhs.y, lhs;
    }

    template <typename CoordT>
    inline Vector<CoordT> multiply(Vector<CoordT> const& lhs, const Vector<CoordT>& rhs)
    {
        Vector<CoordT> v;
        v.x = lhs.x*rhs.x - lhs.y*rhs.y;
        v.y = lhs.x*rhs.y + lhs.y*rhs.x;
        return v;
    }

    template <typename CoordT>
    inline Vector<CoordT> dotcross(Vector<CoordT> const& lhs, const Vector<CoordT>& rhs)
    {
        Vector<CoordT> v;
        v.x = lhs.x*rhs.x + lhs.y*rhs.y;
        v.y = lhs.x*rhs.y - lhs.y*rhs.x;
        return v;
    }

    template <typename CoordT>
    inline const Point<CoordT>& operator +=(Point<CoordT>& lhs, const Vector<CoordT>& rhs)
    {
        return lhs.x += rhs.x, lhs.y += rhs.y, lhs;
    }

    template <typename CoordT>
    inline const Point<CoordT>& operator -=(Point<CoordT>& lhs, const Vector<CoordT>& rhs)
    {
        return lhs.x -= rhs.x, lhs.y -= rhs.y, lhs;
    }

    template <typename CoordT>
    inline Point<CoordT> operator +(const Point<CoordT>& lhs, const Vector<CoordT>& rhs)
    {
        const Point<CoordT> result = {lhs.x + rhs.x, lhs.y + rhs.y};
        return result;
    }

    template <typename CoordT>
    inline Point<CoordT> operator -(const Point<CoordT>& lhs, const Vector<CoordT>& rhs)
    {
        const Point<CoordT> result = {lhs.x - rhs.x, lhs.y - rhs.y};
        return result;
    }

    inline real_t norm(real_t bx, real_t by)
    {
        return sqrtf(bx * bx + by * by);
    }

    inline real_t distance(real_t ax, real_t ay, real_t bx, real_t by)
    {
        return norm(bx - ax, by - ay);
    }

    inline double norm(double bx, double by)
    {
        return sqrt(bx * bx + by * by);
    }

    inline real_t distance(const Point_r& lhs, const Point_r& rhs)
    {
        return norm(lhs.x - rhs.x, lhs.y - rhs.y);
    }

    inline real_t norm(const Vector<float>& v) // real_t
    {
        return norm(v.x, v.y);
    }

    inline double norm(const Vector<double>& v)
    {
        return norm(v.x, v.y);
    }

    template <typename CoordT>
    inline CoordT distance(const Point<CoordT>& a, const Point<CoordT>& b)
    {
        return distance(a.x - b.x, a.y - b.y);
    }

    template <typename CoordT>
    inline Point<CoordT> create_point(CoordT x, CoordT y)
    {
        Point<CoordT> p = {x, y};
        return p;
    }

    template <typename CoordT>
    inline Vector<CoordT> create_vector(CoordT dx, CoordT dy)
    {
        Vector<CoordT> v = {dx, dy};
        return v;
    }
}
