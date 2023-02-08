#pragma once
#include <utility>

#include <plotting/generators/generator.h>
#include <limits>

namespace plotting
{
    namespace pipeline
    {
        template<typename Func>
        struct Filterer: agge::pipeline::terminal<Func>
        {};

        template<typename Func>
        struct Filter: public Filterer<Filter<Func>>
        {
            template<typename F>
            Filter(F&& f) : select(std::forward<F>(f)) {}

            bool operator()(auto const& x) const { return select(x); }
            Func  mutable select;
        };

        template<typename G, typename F>
        class FilterGenerator: public Generator<FilterGenerator<G, F>>
        {
            using UnderlyingIterator = std::remove_cv_t<decltype(std::begin(std::declval<G>()))>;
            using UnderlyingSentinel = std::remove_cv_t<decltype(std::end(std::declval<G>()))>;

            using from_t = std::remove_cvref_t<decltype(*std::begin(std::declval<G>()))>;
        public:
            struct Sentinel {};

            template<typename TG, typename TF>
            FilterGenerator(TG&& g, TF&& f): gen(std::forward<TG>(g)), select(std::forward<TF>(f)) {}

            class Iterator
            {
                friend class FilterGenerator;
                Iterator(FilterGenerator const& ref)
                    : _it(std::begin(ref.gen))
                    , _end(std::end(ref.gen))
                    , ref(ref)
                {
                    next();
                }
            public:

                from_t const& operator*() const { return cached; }
                Iterator& operator++() { if(_it!=_end) ++_it; next(); return *this; }

                bool operator!=(Sentinel) const { return _it != _end; }
                bool operator==(Sentinel) const { return _it == _end; }
            private:
                void next()
                {
                    while(_it!=_end && !ref.select(cached = *_it))
                       ++_it;
                }

                UnderlyingIterator         _it;
                UnderlyingSentinel         _end;
                FilterGenerator   const&   ref;
                from_t           mutable   cached {};
            };

            Iterator begin() const { return Iterator(*this); }
            Sentinel end() const { return {}; }

            G           gen;
            F  mutable  select;
        };

        template<typename G, typename F>
        FilterGenerator<G const&, F> operator/(Generator<G> const& g, Filterer<F> const& f)
        {
            return {g._get_(), f._get_()};
        }

        template<typename G, typename F>
        FilterGenerator<G const&, F> operator/(Generator<G> const& g, Filterer<F>&& f)
        {
            return {g._get_(), std::move(f)._get_()};
        }

        template<typename G, typename F>
        FilterGenerator<G, F> operator/(Generator<G>&& g, Filterer<F>&& f)
        {
            return {std::move(g)._get_(), std::move(f)._get_()};
        }

        template<typename G, typename F>
        FilterGenerator<G, F> operator/(Generator<G>&& g, Filterer<F> const& f)
        {
            return {std::move(g)._get_(), f._get_()};
        }

        template<typename F>
        auto filter(F const& f) { return pipeline::Filter<F const&>{f}; }

        template<typename F>
        auto filter(F&& f) { return pipeline::Filter<F>{ std::move(f)}; }

        template<typename F>
        auto filter(pipeline::Filterer<F>&&)
        {
            assert(false && "Trying to wrap already Filterer in filter!");
        }

        template<typename F>
        auto filter(pipeline::Filterer<F> const&)
        {
            assert(false && "Trying to wrap already Filterer in filter!");
        }
    }

    using pipeline::filter;


    namespace filters
    {
        struct FarEnough: public pipeline::Filterer<FarEnough>
        {
            FarEnough(agge::real_t eps): eps(eps) {}

            bool operator()(agge::point_r const& cur) const
            {
                bool const far = fabs(cur.x - prev.x) + fabs(cur.y - prev.y) > eps;
                if(far)
                    prev = cur;
                return far;
            }

            bool operator()(agge::polyline::Item const& cur) const
            {
                bool const far = fabs(cur.point.x - prev.x) + fabs(cur.point.y - prev.y) > eps;
                if(far)
                    prev = cur.point;
                return far;
            }

            agge::real_t  const   eps {0.5f};
            agge::point_r mutable prev{-std::numeric_limits<agge::real_t>::infinity(),
                                       -std::numeric_limits<agge::real_t>::infinity()};
        };

        struct FarEnough_TimeSeries: public pipeline::Filterer<FarEnough_TimeSeries>
        {
            FarEnough_TimeSeries(agge::real_t eps) : eps(eps) {}

            bool operator()(agge::point_r const& cur) const
            {
                bool far = fabs(cur.x - prev.x) + fabs(cur.y - prev.y) > eps;
                if(far)
                    prev = cur;
                return far;
            }

            bool operator()(agge::polyline::Item const& cur) const
            {
                bool const far = fabs(cur.point.x - prev.x) + fabs(cur.point.y - prev.y) > eps;
                if(far)
                    prev = cur.point;
                return far;
            }

            agge::real_t  const   eps{0.5f};
            agge::point_r mutable prev{-std::numeric_limits<agge::real_t>::infinity(),
                                       -std::numeric_limits<agge::real_t>::infinity()};
        };

        struct SkipOverPeriod: public pipeline::Filterer<SkipOverPeriod>
        {
            SkipOverPeriod(int p, int o = 0): period(p), offset(o) {}
            SkipOverPeriod(SkipOverPeriod&&) = default;
            SkipOverPeriod(SkipOverPeriod const&) = default;

            int const   period = 10;
            int mutable offset = 0;

            bool operator()(agge::point_r const&) const
            {
                bool keep = offset != 0;
                if(++offset >= period)
                    offset = 0;
                return keep;
            }
        };
    }
}