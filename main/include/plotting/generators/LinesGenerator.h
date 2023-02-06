#pragma once
#include "EntitiesGenerator.h"
#include "PointsGenerator.h"

namespace plotting
{
    struct LineData
    {
        agge::point_r start{};
        agge::point_r end{};
    };

    template<typename PointsGenerator>
    struct ParallelLinesGenerator
    {
        PointsGenerator points;
        agge::vector_r  direction{};

        using Sentinel = PointsGenerator::Sentinel;

        class Iterator
        {
        public:
            Iterator(ParallelLinesGenerator const& ref)
                : _it(ref.points.begin()), ref(ref)
            {}

            LineData operator*() const
            {
                auto const p = *_it;
                return {p, p + ref.direction};
            }

            Iterator& operator++()
            {
                ++_it;
                return *this;
            }

            bool operator!=(Sentinel const& s) const { return _it != s; }
        private:
            PointsGenerator::Iterator       _it;
            ParallelLinesGenerator const&   ref;
        };
        Iterator begin() const { return Iterator(*this); }
        Sentinel end()   const { return points.end(); }
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