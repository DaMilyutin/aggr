#pragma once
#include <palgebra/algebra/rules.h>
#include <palgebra/mediators/Path.h>

namespace agge
{
    namespace elements
    {
        // point generators for
        //  intermediary points

        class Vertex: public rules::PointGenerator<Vertex>
        {
        public:
            static constexpr real_t granularity = 5.0f;

            Vertex(Point_r p)
                : point(p)
            {}

            Point_r point;
        };
    }

    namespace rules
    {
        template<typename R>
        R& operator<<(Consumer<R>& ras, elements::Vertex const& v)
        {
            R& the_ras = ras._get_();
            the_ras.line_to(v.point);
            return the_ras;
        }

        template<typename R>
        R& operator<<(Consumer<R>& ras, rules::Start<elements::Vertex> const& v)
        {
            R& the_ras = ras._get_();
            the_ras.move_to(v.under.point);
            return the_ras;
        }

        template<typename R>
        R& operator<<(Consumer<R>& ras, rules::Start<elements::Vertex const&> const& v)
        {
            R& the_ras = ras._get_();
            the_ras.move_to(v.under.point);
            return the_ras;
        }
    }
}