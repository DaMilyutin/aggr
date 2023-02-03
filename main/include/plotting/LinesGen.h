#pragma once
#include <agge/color.h>
#include <agge/primitives/figures.h>
#include <agge/primitives/dash.h>
#include <agge/primitives/stroke.h>
#include <agge/primitives/stroke_features.h>

#include <agge/rendering/platform.h>
#include <agge/rendering/renderer.h>

#include <plotting/Canvas.h>

namespace plotting
{
    struct LineData
    {
        agge::point_r start {};
        agge::point_r end   {};
    };

    struct Selector_Any
    {
        bool operator()(int) const { return true; }
    };

    struct Selector_SkipOverPeriod
    {
        int period = 10;
        int offset = 0;

        bool operator()(int x) const
        {
            return (x-offset)%period != 0;
        }
    };

    template<typename Selector = Selector_Any>
    struct ParallelLinesGenerator
    {
        LineData      initial   {};
        agge::point_r direction {};
        int           start     {};
        int           number    {};
        Selector      select    {};

        class Sentinel
        {
        public:
            int const number;
        };

        class Iterator
        {
        public:
            Iterator(ParallelLinesGenerator const& ref)
                : data(ref.initial), i(ref.start), ref(ref)
            {
                next_sel();
            }

            LineData const& operator*() const { return data; }

            Iterator& operator++()
            {
                skip();
                next_sel();
                return *this;
            }

            bool operator!=(Sentinel const& s) const { return i < s.number; }
        private:
            void skip()
            {
                ++i;
                data.start.x += ref.direction.x;
                data.start.y += ref.direction.y;
                data.end.x   += ref.direction.x;
                data.end.y   += ref.direction.y;
            }

            void next_sel()
            {
                while(!ref.select(i))
                    skip();
            }

            LineData                      data;
            int                           i;
            ParallelLinesGenerator const& ref;
        };
        Iterator begin() const { return Iterator(*this); }
        Sentinel end()   const { return {number}; }
    };

    template<typename Generator, typename Maker>
    class LinesGenerator
    {
    public:
        Generator   gen;
        Maker       maker;
        agge::color color;
    };

    template<typename S, typename Rn, typename Rs, typename Generator, typename Maker>
    plotting::Canvas<S, Rn, Rs>& operator<<(plotting::Canvas<S, Rn, Rs>& c,
        LinesGenerator<Generator, Maker> const& lines)
    {
        c << reset;
        for(LineData const& l : lines.gen)
            c << lines.maker(l);
        c << lines.color;
        return c;
    }

    struct SimpleLineMaker
    {
        agge::line operator()(LineData const& l) const
        {
            return agge::line(l.start.x, l.start.y, l.end.x, l.end.y);
        }
    };

    struct StylishLineMaker
    {
        agge::stroke& style;
        auto operator()(LineData const& l) const
        {
            return agge::line(l.start.x, l.start.y, l.end.x, l.end.y)/style;
        }
    };

    class FancyLineMaker: SimpleLineMaker
    {
        SimpleLineMaker simple;
        agge::dash& dash;
        agge::stroke& style;
        auto operator()(LineData const& l) const
        {
            return agge::line(l.start.x, l.start.y, l.end.x, l.end.y)/dash/style;
        }
    };
}