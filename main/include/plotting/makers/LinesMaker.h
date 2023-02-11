#pragma once

#include <plotting/piping.h>
#include <plotting/types/point_types.h>

namespace plotting
{

    struct LinesInDirection: public piping::Transform<LinesInDirection>
    {
        LinesInDirection(agge::vector_r v): direction(v) {}

        Segment<agge::point_r> operator()(agge::point_r const& p) const { return { p, p + direction }; };
        agge::vector_r          direction{};
    };

    template<typename P>
    struct Lines: public piping::TransformOr<Lines<P>>
    {
        Lines() = default;

        std::optional<Segment<P>> operator()(P const& p) const
        {
            if(!prev)
                return prev = p, std::nullopt;
            P start = *prev;
            prev = p;
            return Segment<P>{start, p};
        };

        template<typename S>
        bool feed(S& sink, P const& p) const
        {
            if(!prev)
                return prev = p, true;
            P start = *prev;
            prev = p;
            return sink.consume(Segment<P>{start, p});
        };

        std::optional<P> mutable prev;
    };


    struct LinesMargined: public piping::TransformOr<LinesMargined>
    {
        LinesMargined() = default;
        LinesMargined(agge::real_t marg): margin(marg) {}

        std::optional<Segment<port_t>> operator()(Segment<port_t> const& s) const
        {
            port_diff_t direction = (s.end - s.start)*0.5f;
            port_t middle = s.start;
            middle += direction;
            auto const n = agge::norm(direction);
            auto const l = (n-margin);
            if(l <= 0.0f)
                return std::nullopt;
            direction *= (l/n);
            return Segment{middle - direction, middle + direction};
        };

        template<typename Sink>
        bool feed(Sink& sink, Segment<port_t> const& s) const
        {
            port_diff_t direction = (s.end - s.start)*0.5f;
            port_t middle = s.start;
            middle += direction;
            auto const n = agge::norm(direction);
            auto const l = (n-margin);
            if(l <= 0.0f)
                return true;
            direction *= (l/n);
            return sink.consume(Segment<agge::point_r>{middle - direction, middle + direction});
        };

        std::optional<Segment<repr_t>> operator()(Segment<repr_t> const& s) const
        {
            repr_diff_t direction = (s.end - s.start)*0.5;
            repr_t middle = s.start;
            middle += direction;
            auto const n = agge::norm(direction);
            auto const l = (n-margin);
            if(l <= 0.0)
                return std::nullopt;
            direction *= (l/n);
            return Segment{middle - direction, middle + direction};
        };

        template<typename Sink>
        bool feed(Sink& sink, Segment<repr_t> const& s) const
        {
            repr_diff_t direction = (s.end - s.start)*0.5;
            repr_t middle = s.start;
            middle += direction;
            auto const n = agge::norm(direction);
            auto const l = (n-margin);
            if(l <= 0.0)
                return true;
            direction *= (l/n);
            return sink.consume(Segment{middle - direction, middle + direction});
        };

        agge::real_t                 margin = 10.0f;
    };

    struct SimpleLineMaker: public piping::Transform<SimpleLineMaker>
    {
        SimpleLineMaker() = default;

        agge::line operator()(Segment<port_t> const& l) const
        {
            return agge::line(l.start.x, l.start.y, l.end.x, l.end.y);
        }
    };

    struct StylingStroke: public piping::Transform<StylingStroke>
    {
        StylingStroke(agge::stroke& s): style(s) {}

        agge::stroke& style;
        auto operator()(Segment<port_t> const& l) const
        {
            return agge::line(l.start.x, l.start.y, l.end.x, l.end.y)/style;
        }
    };

    struct StylingStrokeDash: public piping::Transform<StylingStrokeDash>
    {
        StylingStrokeDash(agge::stroke& s, agge::dash& d): style(s), dash(d) {}

        agge::stroke& style;
        agge::dash&   dash;

        auto operator()(Segment<port_t> const& l) const
        {
            return agge::line(l.start.x, l.start.y, l.end.x, l.end.y)/dash/style;
        }
    };

    inline auto styling(agge::stroke& s)                { return StylingStroke(s); }
    inline auto styling(agge::stroke& s, agge::dash& d) { return StylingStrokeDash(s, d); }
}