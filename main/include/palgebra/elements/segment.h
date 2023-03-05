#pragma once
#include <palgebra/algebra/rules.h>
#include <agge/types/point.h>

namespace agge
{
    namespace elements
    {
        // point generators for
        //  intermediary points

        class Segment: public rules::PointGenerator<Segment>
        {
        public:
            Segment(Point_r s, Point_r e)
                : start(s), end(e)
            {}

            Segment reversed() const { return Segment{end, start}; }

            Point_r start;
            Point_r end;
        };
    }


    namespace rules
    {
        template<typename R>
        R& operator<<(Consumer<R>& ras, elements::Segment const& s)
        {
            R& the_ras = ras._get_();
            the_ras.line_to(s.start);
            the_ras.line_to(s.end);
            return the_ras;
        }

        template<typename R>
        R& operator<<(Consumer<R>& ras, rules::Start<elements::Segment> const& s)
        {
            R& the_ras = ras._get_();
            the_ras.move_to(s.under.start);
            the_ras.line_to(s.under.end);
            return the_ras;
        }

        template<typename R>
        R& operator<<(Consumer<R>& ras, rules::Start<elements::Segment const&> const& s)
        {
            R& the_ras = ras._get_();
            the_ras.move_to(s.under.start);
            the_ras.line_to(s.under.end);
            return the_ras;
        }
    }
}