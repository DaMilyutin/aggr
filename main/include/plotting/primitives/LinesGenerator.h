#pragma once

#include <plotting/generators/transform.h>

namespace plotting
{
    struct LineData
    {
        agge::point_r start{};
        agge::point_r end{};
    };

    struct LinesInDirection: public pipeline::Transformer<LinesInDirection>
    {
        LinesInDirection(agge::vector_r v): direction(v) {}

        LineData operator()(agge::point_r const& p) const { return { p, p + direction }; };
        agge::vector_r  direction{};
    };




    struct SimpleLineMaker: public pipeline::Transformer<SimpleLineMaker>
    {
        SimpleLineMaker() = default;

        agge::line operator()(LineData const& l) const
        {
            return agge::line(l.start.x, l.start.y, l.end.x, l.end.y);
        }
    };

    struct StylishLineMaker: public pipeline::Transformer<StylishLineMaker>
    {
        StylishLineMaker(agge::stroke& s): style(s) {}

        agge::stroke& style;
        auto operator()(LineData const& l) const
        {
            return agge::line(l.start.x, l.start.y, l.end.x, l.end.y)/style;
        }
    };

    struct FancyLineMaker: public pipeline::Transformer<FancyLineMaker>
    {
        FancyLineMaker(agge::dash& d, agge::stroke& s): dash(d), style(s) {}

        agge::dash& dash;
        agge::stroke& style;
        auto operator()(LineData const& l) const
        {
            return agge::line(l.start.x, l.start.y, l.end.x, l.end.y)/dash/style;
        }
    };
}