#pragma once

#include <agge/types.h>
#include <agge/math/bezier.h>
#include <agge/primitives/algebra/rules.h>

#include <utility>

namespace agge
{
    class Interval
    {
    public:
        Interval(real_t step);

        int vertex(real_t* t);
    protected:
        real_t _t, _step;
        int _stage;
    };

    template<typename ACurve, typename AInterval = Interval>
    class curve_generator: public rules::PointGenerator<curve_generator<ACurve, AInterval>>
    {
    public:
        template<typename TC, typename TI>
        curve_generator(TC&& c, TI&& i): curve(std::forward<TC>(c)), interval(std::forward<TI>(i)) {}

        void rewind(unsigned id);
        int vertex(real_t* x, real_t* y) { Point_r p; int flag = vertex(&p); *x = p.x; *y = p.y; return flag; };
        int vertex(Point_r* p)
        {
            real_t t;
            int const stage = interval.vertex(&t);
            *p = curve.at(t);
            return stage;
        }
    protected:
        ACurve    curve;
        AInterval interval;
    };

    using qbezier = curve_generator<qbezier_interp>;
    using cbezier = curve_generator<cbezier_interp>;

    inline auto operator/(Interval i, qbezier_interp c)
    {
        return qbezier(c, i);
    }

    inline auto operator/(Interval i, cbezier_interp c)
    {
        return cbezier(c, i);
    }

    class arc: public rules::PointGenerator<arc>
    {
    public:
        arc(real_t cx, real_t cy, real_t r, real_t start, real_t end, real_t da = 0.05f);

        void rewind(unsigned id);
        int vertex(real_t* x, real_t* y);

    private:
        real_t _a;
        int _stage;
        real_t _cx, _cy, _r, _start, _end, _step;
    };
}
