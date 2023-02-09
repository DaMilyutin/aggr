#pragma once
#include <plotting/generators/abstract.h>
#include <limits>

namespace plotting
{
    namespace pipeline
    {
        template<typename Func>
        struct Filter: Link<Func>
        {};

        template<typename Func>
        struct FilterWrap: public Filter<FilterWrap<Func>>
        {
            template<typename F>
            FilterWrap(F&& f) : select(std::forward<F>(f)) {}

            bool operator()(auto const& x) const { return select(x); }

            template<typename S, typename E>
            bool feed(Sink<S>& sink, E&& e) const
            {
                if(!select(e))
                    return false;
                return (sink._get_()) << FWD(e);
            }

            Func  mutable select;
        };

        template<typename G, typename F>
        class FilterGenerator: public Yield<FilterGenerator<G, F>>
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
        FilterGenerator<G const&, F> operator/(Yield<G> const& g, Filter<F> const& f)
        {
            return {g._get_(), f._get_()};
        }

        template<typename G, typename F>
        FilterGenerator<G const&, F> operator/(Yield<G> const& g, Filter<F>&& f)
        {
            return {g._get_(), std::move(f)._get_()};
        }

        template<typename G, typename F>
        FilterGenerator<G, F> operator/(Yield<G>&& g, Filter<F>&& f)
        {
            return {std::move(g)._get_(), std::move(f)._get_()};
        }

        template<typename G, typename F>
        FilterGenerator<G, F> operator/(Yield<G>&& g, Filter<F> const& f)
        {
            return {std::move(g)._get_(), f._get_()};
        }

        template<typename F>
        auto filter(F const& f) { return pipeline::FilterWrap<F const&>{f}; }

        template<typename F>
        auto filter(F&& f) { return pipeline::FilterWrap<F>{ std::move(f)}; }

        template<typename F>
        auto filter(pipeline::Filter<F>&&)
        {
            assert(false && "Trying to wrap already Filterer in filter!");
        }

        template<typename F>
        auto filter(pipeline::Filter<F> const&)
        {
            assert(false && "Trying to wrap already Filterer in filter!");
        }
    }

    using pipeline::filter;
}