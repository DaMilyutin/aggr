#pragma once
#include <utility>

#include <plotting/generators/PointsGenerator.h>
#include <agge/primitives/pipeline.h>
#include <agge/types.h>

#include <limits>

namespace plotting
{
    namespace pipeline
    {
        template<typename Func>
        struct Filter: agge::pipeline::terminal<Func>
        {
            Func select;
        };

        template<typename G, typename F>
        class FilterGenerator: public PointsGenerator<FilterGenerator<G, F>, agge::point_r>
        {
            using UnderlyingIterator = std::remove_cv_t<decltype(std::begin(std::declval<G>()))>;
            using UnderlyingSentinel = std::remove_cv_t<decltype(std::end(std::declval<G>()))>;

            using from_t = std::remove_cvref_t<decltype(*std::begin(std::declval<G>()))>;
            static_assert(std::is_same_v<from_t, port_t>,
                "generator from must generate port_t (aka agge::point_r) or ref of port_t");
        public:
            struct Sentinel {};

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

                agge::point_r const& operator*() const { return cached; }
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
                FilterGenerator const&     ref;
                agge::point_r     mutable  cached {};
            };

            Iterator begin() const { return Iterator(*this); }
            Sentinel end() const { return {}; }

            G     gen;
            F     select;
        };

        template<typename G, typename F>
        FilterGenerator<G const&, F> operator/(PointsGenerator<G, port_t> const& g, Filter<F> const& f)
        {
            return {{}, g._get_(), f.select};
        }

        template<typename G, typename F>
        FilterGenerator<G const&, F> operator/(PointsGenerator<G, port_t> const& g, Filter<F>&& f)
        {
            return {{}, g._get_(), std::move(f.select)};
        }

        template<typename G, typename F>
        FilterGenerator<G, F> operator/(PointsGenerator<G, port_t>&& g, Filter<F>&& f)
        {
            return {{}, std::move(g._get_()), std::move(f.select)};
        }

        template<typename G, typename F>
        FilterGenerator<G, F> operator/(PointsGenerator<G, port_t>&& g, Filter<F> const& f)
        {
            return {{}, std::move(g._get_()), f.select};
        }
    }

    namespace filters
    {
        template<typename F>
        auto filter(F const& f) { return pipeline::Filter<F const&>{ {}, f}; }

        template<typename F>
        auto filter(F&& f) { return pipeline::Filter<F>{ {}, std::move(f)}; }

        struct FarEnough
        {
            bool operator()(agge::point_r const& cur) const
            {
                bool const far = fabs(cur.x - prev.x) + fabs(cur.y - prev.y) > eps;
                if(far)
                    prev = cur;
                return far;
            }

            agge::real_t  const   eps {0.5f};
            agge::point_r mutable prev{-std::numeric_limits<agge::real_t>::infinity(),
                                       -std::numeric_limits<agge::real_t>::infinity()};
        };

        struct FarEnough_TimeSeries
        {
            bool operator()(agge::point_r const& cur) const
            {
                bool far = fabs(cur.x - prev.x) + fabs(cur.y - prev.y) > eps;
                if(far)
                    prev = cur;
                return far;
            }

            agge::real_t  const   eps{0.5f};
            agge::point_r mutable prev{-std::numeric_limits<agge::real_t>::infinity(),
                                       -std::numeric_limits<agge::real_t>::infinity()};
        };

        struct SkipOverPeriod
        {
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