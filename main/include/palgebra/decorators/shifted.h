#pragma once

#include <palgebra/algebra/algebra.h>
#include <palgebra/algebra/decorated.h>
#include <palgebra/algebra/concat.h>
#include <palgebra/mediators/stored.h>
#include <palgebra/mediators/Path.h>

namespace agge
{
    namespace rules
    {
        struct OrthoShift: Decorator<OrthoShift>
        {
            OrthoShift(real_t off)
                : offset(off)
            {}

            real_t offset = 0.0f;
        };

        template<typename R, typename P>
        R& operator<<(Consumer<R>& ras, rules::Decorated<P, OrthoShift> const& v)
        {
            R& the_ras = ras._get_();
            P const& the_points = v.points;
            auto b = the_points.begin();
            auto e = the_points.end();
            if(b == e)
                return the_ras;
            the_ras.move_to(*b);
            for(++b; b != e; ++b)
                the_ras.line_to(*b);
            return the_ras;
        }

        template<typename R, typename P>
        R& operator<<(Consumer<R>& ras, rules::Decorated<rules::Concat<P>, OrthoShift> const& v)
        {
            R& the_ras = ras._get_();
            P const& the_points = v.points;
            auto b = the_points.begin();
            auto e = the_points.end();
            for(; b != e; ++b)
                the_ras.line_to(*b);
            return the_ras;
        }
    }
}