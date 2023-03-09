#pragma once
#include <palgebra/algebra/rules.h>
#include <palgebra/mediators/Path.h>

namespace agge
{
    namespace elements
    {
        // point generators for
        //  intermediary points

        class Vertex: public rules::Yield<Vertex>
        {
        public:
            static constexpr real_t granularity = 5.0f;

            Vertex(Point_r p)
                : point(p)
            {}

            struct const_iterator
            {
                Point_r const* point = 0;

                const_iterator& operator++() { point = 0; return *this; }
                Point_r const& operator*() const { return *point; }

                bool operator == (const_iterator rhs) const { return point == rhs.point; }
                bool operator != (const_iterator rhs) const { return point != rhs.point; }
            };

            const_iterator begin() const { return {&point}; }
            const_iterator end()   const { return {0}; }

            Point_r point;
        };
    }

    namespace rules
    {
        template<typename R>
        R& operator<<(Sink<R>& ras, elements::Vertex const& v)
        {
            R& the_ras = ras._get_();
            the_ras.line_to(v.point);
            return the_ras;
        }

        template<typename R>
        R& operator<<(Sink<R>& ras, rules::Start<elements::Vertex> const& v)
        {
            R& the_ras = ras._get_();
            the_ras.move_to(v.under.point);
            return the_ras;
        }

        template<typename R>
        R& operator<<(Sink<R>& ras, rules::Start<elements::Vertex const&> const& v)
        {
            R& the_ras = ras._get_();
            the_ras.move_to(v.under.point);
            return the_ras;
        }
    }
}