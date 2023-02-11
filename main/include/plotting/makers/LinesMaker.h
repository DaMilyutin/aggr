#pragma once

#include <plotting/piping.h>
#include <plotting/types/point_types.h>

namespace plotting
{
    struct LineData
    {
        agge::point_r start{};
        agge::point_r end{};
    };

    struct LinesInDirection: public piping::Transform<LinesInDirection>
    {
        LinesInDirection(agge::vector_r v): direction(v) {}

        LineData operator()(agge::point_r const& p) const { return { p, p + direction }; };
        agge::vector_r  direction{};
    };

    struct LinesMargined: public piping::TransformOr<LinesMargined>
    {
        LinesMargined() = default;

        std::optional<LineData> operator()(agge::point_r const& p) const
        {
            if(!prev)
                return prev = p, std::nullopt;
            direction = (p - *prev)*0.5f;
            middle = *prev;
            middle += direction;
            auto const n = agge::norm(direction);
            direction *= ((n-margin)/n);
            prev = p;
            return LineData{middle - direction, middle + direction};
        };


        agge::real_t                 margin = 10.0f;

        agge::vector_r               mutable direction{};
        agge::point_r                mutable middle{};
        std::optional<agge::point_r> mutable prev;
    };

    struct SimpleLineMaker: public piping::Transform<SimpleLineMaker>
    {
        SimpleLineMaker() = default;

        agge::line operator()(LineData const& l) const
        {
            return agge::line(l.start.x, l.start.y, l.end.x, l.end.y);
        }
    };

    struct StylishLineMaker: public piping::Transform<StylishLineMaker>
    {
        StylishLineMaker(agge::stroke& s): style(s) {}

        agge::stroke& style;
        auto operator()(LineData const& l) const
        {
            return agge::line(l.start.x, l.start.y, l.end.x, l.end.y)/style;
        }
    };

    struct FancyLineMaker: public piping::Transform<FancyLineMaker>
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