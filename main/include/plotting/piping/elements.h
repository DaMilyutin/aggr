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


        template<typename F> auto filter(F const& f) { return ylems::elements::filter<terminal>(f); }
        template<typename F> auto filter(F&& f) { return ylems::elements::filter<terminal>(std::move(f)); }

        template<typename F> auto transform(F const& f) { return ylems::elements::transform<terminal>(f); }
        template<typename F> auto transform(F&& f) { return ylems::elements::transform<terminal>(std::move(f)); }

        template<typename F> auto transform_or(F const& f) { return ylems::elements::transform_or<terminal>(f); }
        template<typename F> auto transform_or(F&& f) { return ylems::elements::transform_or<terminal>(std::move(f)); }

        template<typename F, typename G> auto transform_or(F&& f, G&& g) { return ylems::elements::transform_or<terminal>(FWD(f), FWD(g)); }

        template<typename... T> auto zip(T&&... f) { return ylems::elements::zip<terminal>(FWD(f)...); }
    }

    using piping::iota;
    using piping::linspace;
    using piping::range;

    using piping::filter;
    using piping::transform;
    using piping::transform;

    using piping::zip;
}