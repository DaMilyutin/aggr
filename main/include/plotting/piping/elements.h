#pragma once
#pragma once
#include "rules.h"
#include <ylems/elements.h>

namespace plotting
{
    namespace piping
    {
        template<typename T, typename D, typename I>
                                         auto iota(T t, D d, I i) { return ylems::elements::iota<terminal>(t, d, i); }
        template<typename T, typename D> auto iota(T t, D d) { return ylems::elements::iota<terminal>(t, d); }

        template<typename T, typename I> auto linspace(T b, T e, I i) { return ylems::elements::linspace<terminal>(b, e, i); }

        template<typename T>             auto range(T b, T e, T step) { return ylems::elements::range<terminal>(b, e, step); }
        template< typename T>            auto range(T b, T e) { return ylems::elements::range<terminal>(b, e); }

        template<typename Y> auto yield(Y&& y) { return ylems::elements::yield<terminal>(FWD(y)); }
        template<typename... T> auto zip(T&&... f) { return ylems::elements::zip<terminal>(FWD(f)...); }

        template<typename F> auto filter(F&& f) { return ylems::elements::filter<terminal>(FWD(f)); }
        template<typename I> auto take(I i) { return ylems::elements::take<terminal>(i); }
        template<typename I> auto drop(I i) { return ylems::elements::drop<terminal>(i); }

        template<typename F> auto transform(F&& f) { return ylems::elements::transform<terminal>(FWD(f)); }
        template<typename F> auto transform_or(F&& f) { return ylems::elements::transform_or<terminal>(FWD(f)); }
        template<typename F, typename G> auto transform_or(F&& f, G&& g) { return ylems::elements::transform_or<terminal>(FWD(f), FWD(g)); }
    }

    using piping::iota;
    using piping::linspace;
    using piping::range;

    using piping::yield;
    using piping::zip;

    using piping::filter;
    using piping::take;
    using piping::drop;

    using piping::transform;
    using piping::transform_or;
}