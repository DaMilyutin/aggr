#pragma once

#include <palgebra/algebra/algebra.h>
#include <palgebra/algebra/decorated.h>
#include <palgebra/algebra/start.h>
#include <palgebra/mediators/stored.h>
#include <palgebra/mediators/Path.h>

#include <palgebra/elements/Segment.h>

namespace agge
{
    namespace decorators
    {
        struct OrthoShift: rules::Decorator<OrthoShift>
        {
            OrthoShift(real_t off)
                : offset(off)
            {}

            real_t offset = 0.0f;

            elements::Segment from(elements::Segment s, real_t length) const
            {
                real_t const scale = offset/length;
                Vector_r const o{(s.end.y - s.start.y)*scale, -(s.end.x - s.start.x)*scale};
                s.start += o;
                s.end += o;
                return s;
            }

            elements::Segment from(elements::Segment s) const
            {
                real_t const scale = offset/distance(s.start, s.end);
                Vector_r const o{(s.end.y - s.start.y)*scale, -(s.end.x - s.start.x)*scale};
                s.start += o;
                s.end += o;
                return s;
            }
        };
    }


    namespace rules
    {
        //template<typename R, typename E>
        //R& operator<<(Consumer<R>& ras, rules::Decorated<rules::Decorated<P, Stored<>>
        //                                               , decorators::OrthoShift> const& v)
        //{
        //    R& the_ras = ras._get_();
        //    P const& the_points = v.points;
        //    if(the_points.size() <= 1)
        //        return the_ras;

        //    PathAnnotation<Distance> anno(the_points);
        //    auto c = the_points.begin();
        //    auto n = c+1;
        //    auto e = the_points.end();
        //    auto l = anno.data.begin() + 1;
        //    for(; n != e; ++c, ++n, ++l)
        //        the_ras << v.decorator.from(elements::Segment{*c, *n}, *l);
        //    return the_ras;
        //}


        template<typename R, typename P>
        R& operator<<(Consumer<R>& ras, rules::Decorated<P, decorators::OrthoShift> const& v)
        {
            R& the_ras = ras._get_();
            P const& the_points = v.points;

            auto b = the_points.begin();
            auto e = the_points.end();
            if(b == e)
                return the_ras;
            Point_r p;
            Point_r c = *b;
            for(p = c, ++b; b != e; p = c, ++b)
            {
                c = *b;
                the_ras << v.decorator.from(elements::Segment{p, c});
            }
            return the_ras;
        }

        template<typename R, typename P>
        R& operator<<(Consumer<R>& ras, rules::Decorated<rules::Start<P> const&, decorators::OrthoShift> const& v)
        {
            R& the_ras = ras._get_();
            P const& the_points = v.points;

            auto b = the_points.begin();
            auto e = the_points.end();
            if(b == e)
                return the_ras;
            Point_r p;
            Point_r c = *b;
            ++b;
            if(b == e)
                return the_ras;
            c = *b;
            the_ras << start(v.decorator.from(elements::Segment{p, c}));
            for(p = c, ++b; b != e; p = c, ++b)
            {
                c = *b;
                the_ras << v.decorator.from(elements::Segment{p, c});
            }

            return the_ras;
        }

        template<typename R, typename P>
        R& operator<<(Consumer<R>& ras, rules::Start<rules::Decorated<P, decorators::OrthoShift> const&> const& v)
        {
            R& the_ras = ras._get_();
            auto const& decorator = v.under.decorator;
            P const& the_points = v.under.points;

            auto b = the_points.begin();
            auto e = the_points.end();
            if(b == e)
                return the_ras;
            Point_r p;
            Point_r c = *b;
            ++b;
            if(b == e)
                return the_ras;
            p = c;
            c = *b;
            the_ras << start(decorator.from(elements::Segment{p, c}));
            for(p = c, ++b; b != e; p = c, ++b)
            {
                c = *b;
                the_ras << decorator.from(elements::Segment{p, c});
            }

            return the_ras;
        }


    }
}