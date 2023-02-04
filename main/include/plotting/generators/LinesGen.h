#pragma once
#include "EntitiesGen.h"

namespace plotting
{
    struct LineData
    {
        agge::point_r start{};
        agge::point_r end{};
    };

    template<typename Selector = Selector_Any>
    struct ParallelLinesGenerator
    {
        LineData       initial{};
        agge::vector_r direction{};
        int            start{};
        int            number{};
        Selector       select{};

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
                data.start += ref.direction;
                data.end   += ref.direction;
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

    struct FancyLineMaker
    {
        agge::dash& dash;
        agge::stroke& style;
        auto operator()(LineData const& l) const
        {
            return agge::line(l.start.x, l.start.y, l.end.x, l.end.y)/dash/style;
        }
    };
}