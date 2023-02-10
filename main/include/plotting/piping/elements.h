#pragma once
//#include <ylems/elements.h>
//
//namespace plotting
//{
//    namespace piping
//    {
//        template<typename T, typename D, typename I>
//        using Iota = ylems::elements::Iota<T, D, I, Plotting_tag>;
//        template<typename T>    using Transform = ylems::elements::Transform<T, Plotting_tag>;
//        template<typename T>    using TransformOr = ylems::elements::TransformOr<T, Plotting_tag>;
//        template<typename T>    using Filter = ylems::elements::Filter<T, Plotting_tag>;
//        template<typename... T> using ZipYield = ylems::elements::ZipYield<Plotting_tag, T...>;
//
//
//        template<typename T, typename D, typename I>
//        auto iota(T t, D d, I i) { return ylems::elements::iota<Plotting_tag>(t, d, i); }
//        template<typename T, typename D> auto iota(T t, D d) { return ylems::elements::iota<Plotting_tag>(t, d); }
//
//        template<typename T, typename I> auto linspace(T b, T e, I i) { return ylems::elements::linspace<Plotting_tag>(b, e, i); }
//
//        template<typename T>             auto range(T b, T e, T step) { return ylems::elements::range<Plotting_tag>(b, e, step); }
//        template< typename T>            auto range(T b, T e) { return ylems::elements::range<Plotting_tag>(b, e); }
//
//
//        template<typename F> auto filter(F const& f) { return ylems::elements::filter<Plotting_tag>(f); }
//        template<typename F> auto filter(F&& f) { return ylems::elements::filter<Plotting_tag>(std::move(f)); }
//
//        template<typename F> auto transform(F const& f) { return ylems::elements::transform<Plotting_tag>(f); }
//        template<typename F> auto transform(F&& f) { return ylems::elements::transform<Plotting_tag>(std::move(f)); }
//
//        template<typename F> auto transform_or(F const& f) { return ylems::elements::transform_or<Plotting_tag>(f); }
//        template<typename F> auto transform_or(F&& f) { return ylems::elements::transform_or<Plotting_tag>(std::move(f)); }
//
//        template<typename F, typename G> auto transform_or(F&& f, G&& g) { return ylems::elements::transform_or<Plotting_tag>(FWD(f), FWD(g)); }
//
//        template<typename... T> auto zip(T&&... f) { return ylems::elements::zip<Plotting_tag>(FWD(f)...); }
//    }
//
//    using piping::iota;
//    using piping::filter;
//    using piping::transform;
//    using piping::transform;
//}