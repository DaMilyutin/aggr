#pragma once

#include <agge/types.h>
#include <grace/algebra/rules.h>
#include <agge/primitives/path.h>

#include <agge/math/bezier.h>

namespace agge
{
    class line: public grace::rules::Yield<line>
    {
    public:
        line(real_t x1, real_t y1, real_t x2, real_t y2);
        line(Point_r p1, Point_r p2);

        void rewind(unsigned id);
        int vertex(real_t* x, real_t* y);

    private:
        Point_r _points[2];
        unsigned _step;
    };


    class rectangle: public grace::rules::Yield<rectangle>
    {
    public:
        rectangle(real_t x1, real_t y1, real_t x2, real_t y2);
        rectangle(Point_r min, Point_r max);

        void rewind(unsigned id);
        int vertex(real_t* x, real_t* y);

    private:
        Point_r _points[2];
        unsigned _step;
    };


    class rounded_rectangle: public grace::rules::Yield<rounded_rectangle>
    {
    public:
        rounded_rectangle(real_t x1, real_t y1, real_t x2, real_t y2, real_t rx, real_t ry = 0.0f);

        void rewind(unsigned id);
        int vertex(real_t* x, real_t* y);

    private:
        enum { initial = 1, left = 0, right = 2, top = 0, bottom = 4, close = 8, complete = 16 };

    private:
        void setup_bezier(unsigned state);

    private:
        real_t _t, _step;
        cbezier_interp _interp;
        real_t _rx, _ry;
        Point_r _points[2];
        unsigned _state;
    };
}
