#pragma once

#include <palgebra/algebra/algebra.h>
#include <palgebra/algebra/adapted.h>
#include <palgebra/algebra/start.h>
#include <palgebra/mediators/Path.h>

#include <palgebra/elements/Segment.h>

namespace agge
{
    namespace decorators
    {
        struct OrthoShift: rules::TransformOr<OrthoShift>
        {
            OrthoShift(real_t off)
                : offset(off)
            {}

            real_t offset = 0.0f;

            //elements::Segment operator()(elements::Segment s, real_t length) const
            //{
            //    real_t const scale = offset/length;
            //    Vector_r const o{(s.data[1].y - s.data[0].y)*scale, -(s.data[1].x - s.data[0].x)*scale};
            //    s.data[0] += o;
            //    s.data[1] += o;
            //    return s;
            //}

            //elements::Segment operator()(elements::Segment s) const
            //{
            //    real_t const scale = offset/distance(s.data[0], s.data[1]);
            //    Vector_r const o{(s.data[1].y - s.data[0].y)*scale, -(s.data[1].x - s.data[0].x)*scale};
            //    s.data[0] += o;
            //    s.data[1] += o;
            //    return s;
            //}

            //template<size_t N>
            //elements::Segment operator()(ylems::elements::CycleBuffer<Point_r, N> const& b) const
            //{
            //    elements::Segment s{b.back(1), b.back(0)};
            //    return (*this)(s);
            //}

            template<typename S>
            bool feed(S& the_sink, Point_r const& p) const
            {
                buffer.push_back(p);
                if(buffer.size() < 2)
                    return true;
                real_t const scale = offset/distance(buffer.back(1), buffer.back(0));
                Vector_r const o{(buffer.back(0).y - buffer.back(1).y)*scale, -(buffer.back(0).x - buffer.back(1).x)*scale};
                return the_sink.consume(buffer.back(1) + o)
                    && the_sink.consume(buffer.back(0) + o);
            }

            ylems::elements::CycleBuffer<Point_r, 2> mutable buffer;
        };
    }


    namespace rules
    {
        //template<typename R, typename E>
        //R& operator<<(Sink<R>& ras, rules::Decorated<rules::Decorated<P, Stored<>>
        //                                               , decorators::OrthoShift> const& v)
        //{
        //    R& the_ras = ras._get_();
        //    P const& the_gen = v.points;
        //    if(the_gen.size() <= 1)
        //        return the_ras;

        //    PathAnnotation<Distance> anno(the_gen);
        //    auto c = the_gen.begin();
        //    auto n = c+1;
        //    auto e = the_gen.end();
        //    auto l = anno.data.begin() + 1;
        //    for(; n != e; ++c, ++n, ++l)
        //        the_ras << v.decorator.from(elements::Segment{*c, *n}, *l);
        //    return the_ras;
        //}

        //template<typename R, typename P>
        //R& operator<<(Sink<R>& ras, YieldLink<P const&, decorators::OrthoShift const&> const& v)
        //{
        //    R& the_ras = ras._get_();
        //    P const& the_gen = v.yield;
        //    auto b = the_gen.begin();
        //    auto e = the_gen.end();
        //    for(;b != e; ++b)
        //        the_ras << v.link(*b);
        //    return the_ras;
        //}

        //template<typename R, typename P>
        //R& operator<<(Sink<R>& ras, YieldLink<rules::Start<P> const&, decorators::OrthoShift const&> const& v)
        //{
        //    R& the_ras = ras._get_();
        //    P const& the_points = v.yield.under;

        //    auto b = the_points.begin();
        //    auto e = the_points.end();
        //    if(b == e)
        //        return the_ras;
        //    Point_r p;
        //    Point_r c = *b;
        //    ++b;
        //    if(b == e)
        //        return the_ras;
        //    c = *b;
        //    the_ras << start(v.link(elements::Segment{p, c}));
        //    for(p = c, ++b; b != e; p = c, ++b)
        //    {
        //        c = *b;
        //        the_ras << v.link(elements::Segment{p, c});
        //    }

        //    return the_ras;
        //}

        //template<typename R, typename P>
        //R& operator<<(Sink<R>& ras, rules::Start<rules::YieldLink<P const&, decorators::OrthoShift const&> const&> const& v)
        //{
        //    R& the_ras = ras._get_();
        //    auto const& decorator = v.under.link;
        //    P const& the_points = v.under.yield;

        //    auto b = the_points.begin();
        //    auto e = the_points.end();
        //    if(b == e)
        //        return the_ras;
        //    Point_r p;
        //    Point_r c = *b;
        //    ++b;
        //    if(b == e)
        //        return the_ras;
        //    p = c;
        //    c = *b;
        //    the_ras << start(decorator(elements::Segment{p, c}));
        //    for(p = c, ++b; b != e; p = c, ++b)
        //    {
        //        c = *b;
        //        the_ras << decorator(elements::Segment{p, c});
        //    }

        //    return the_ras;
        //}

        //template<typename R, typename P>
        //R& operator<<(Sink<R>& ras, rules::Start<rules::YieldLink<P const&, decorators::OrthoShift> const&> v)
        //{
        //    auto c = v.under.as_const();
        //    return ras << start(c);
        //}


        //template<typename R, typename P>
        //R& operator<<(Sink<R>& ras, YieldLink<P, decorators::OrthoShift> const& v)
        //{
        //    auto const& c = v.as_const();
        //    return ras << c;
        //}


    }
}