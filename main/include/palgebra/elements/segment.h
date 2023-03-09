#pragma once
#include <palgebra/algebra/rules.h>
#include <agge/types/point.h>

namespace agge
{
    namespace elements
    {
        // point generators for
        //  intermediary points

        class Segment: public rules::Yield<Segment>
        {
        public:
            Segment(Point_r s, Point_r e)
                : data{s, e}
            {}

            Segment reversed() const { return Segment{data[1], data[0]}; }

            struct const_iterator
            {
                Point_r const* point = 0;

                const_iterator& operator++() { ++point; return *this; }
                Point_r const& operator*() const { return *point; }

                bool operator == (const_iterator rhs) const { return point == rhs.point; }
                bool operator != (const_iterator rhs) const { return point != rhs.point; }
            };

            const_iterator begin() const { return {+data}; }     // hackhacky: data lie contiguously in memory
            const_iterator end()   const { return {+data + 2}; } // hackhacky: so this should point after end

            Point_r data[2];
        };
    }


    namespace rules
    {
        template<typename R>
        R& operator<<(Sink<R>& ras, elements::Segment const& s)
        {
            R& the_ras = ras._get_();
            the_ras.line_to(s.data[0]);
            the_ras.line_to(s.data[1]);
            return the_ras;
        }

        template<typename R>
        R& operator<<(Sink<R>& ras, rules::Start<elements::Segment> const& s)
        {
            R& the_ras = ras._get_();
            the_ras.move_to(s.under.data[0]);
            the_ras.line_to(s.under.data[1]);
            return the_ras;
        }

        template<typename R>
        R& operator<<(Sink<R>& ras, rules::Start<elements::Segment const&> const& s)
        {
            R& the_ras = ras._get_();
            the_ras.move_to(s.under.data[0]);
            the_ras.line_to(s.under.data[1]);
            return the_ras;
        }
    }
}