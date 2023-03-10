#pragma once

#include <palgebra/algebra/algebra.h>
#include <palgebra/algebra/adapted.h>
#include <palgebra/algebra/start.h>

#include <palgebra/elements/Segment.h>
#include <optional>

namespace agge
{
    namespace elements
    {
        struct Segments: rules::TransformOr<Segments>
        {

            std::optional<elements::Segment> operator()(rules::Start<Point_r const&> const& p) const
            {
                buffer.clear();
                buffer.push_back(p.under);
                start = true;
                return std::nullopt;
            }

            std::optional<elements::Segment> operator()(Point_r const& p) const
            {
                buffer.push_back(p);
                if(buffer.size() < 2)
                    return std::nullopt;
                return elements::Segment{buffer.back(1), buffer.back(0)};
            }

            template<typename S>
            bool feed(S& the_sink, Point_r const& p) const
            {
                buffer.push_back(p);
                if(buffer.size() < 2)
                    return false;
                if(start)
                {
                    start = false;
                    return the_sink.consume(agge::rules::start(elements::Segment{buffer.back(1), buffer.back(0)}));
                }
                return the_sink.consume(elements::Segment{buffer.back(1), buffer.back(0)});
            }

            template<typename S>
            bool feed(S&, rules::Start<Point_r const&> const& p) const
            {
                buffer.clear();
                buffer.push_back(p.under);
                start = true;
                return true;
            }


            ylems::elements::CycleBuffer<Point_r, 2> mutable buffer;
            bool                                     mutable start = false;
        };
    }
}