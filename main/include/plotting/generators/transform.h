#pragma once
#include <plotting/generators/abstract.h>
#include <assert.h>

namespace plotting
{
    namespace pipeline
    {
        template<typename E>
        struct Transform: public Link<E> {};

        template<typename Func>
        struct TransformWrap: public Transform<TransformWrap<Func>>
        {
            template<typename F>
            TransformWrap(F&& f): transform(std::forward<F>(f)) {}

            auto operator()(auto x) const { return transform(x); }

            template<typename S, typename E>
            bool feed(Sink<S>& sink, E&& e) const
            {
                return sink._get_().eat(transform(FWD(e)));
            }

            Func mutable transform;
        };

        template<typename G, typename F>
        class TransformGenerator: public Yield<TransformGenerator<G, F>>
        {
            using UnderlyingIterator = std::remove_cv_t<decltype(std::begin(std::declval<G>()))>;
            using UnderlyingSentinel = std::remove_cv_t<decltype(std::end(std::declval<G>()))>;

            using from_t = std::remove_cvref_t<decltype(*std::begin(std::declval<G>()))>;
        public:
            using Sentinel = UnderlyingSentinel;

            template<typename G, typename T>
            TransformGenerator(G&& g, T&& t): gen(std::forward<G>(g)), transform(std::forward<T>(t)) {}

            class Iterator
            {
                friend class TransformGenerator;
                Iterator(TransformGenerator const& ref)
                    : _it(std::begin(ref.gen))
                    , ref(ref)
                {}
            public:
                Iterator& operator++() { ++_it; return *this; }
                auto operator*() const { return ref.transform(*_it); }
                bool operator!=(Sentinel const& s) const { return _it != s; }
                bool operator==(Sentinel const& s) const { return _it == s; }
            private:
                UnderlyingIterator     _it;
                TransformGenerator const& ref;
            };

            Iterator begin() const { return Iterator(*this); }
            Sentinel end() const { return gen.end(); }

            G     gen;
            F     transform;
        };

        template<typename G, typename F>
        TransformGenerator<G const&, F> operator/(Yield<G> const& g, Transform<F> const& f)
        {
            return {g._get_(), f._get_()};
        }

        template<typename G, typename F>
        TransformGenerator<G const&, F> operator/(Yield<G> const& g, Transform<F>&& f)
        {
            return {g._get_(), std::move(f)._get_()};
        }

        template<typename G, typename F>
        TransformGenerator<G, F> operator/(Yield<G>&& g, Transform<F>&& f)
        {
            return {std::move(g)._get_(), std::move(f)._get_()};
        }

        template<typename G, typename F>
        TransformGenerator<G, F> operator/(Yield<G>&& g, Transform<F> const& f)
        {
            return {std::move(g)._get_(), f._get_()};
        }

        template<typename F>
        auto transform(F const& f) { return pipeline::TransformWrap<F const&>{ f}; }

        template<typename F>
        auto transform(F&& f) { return pipeline::TransformWrap<F>{std::move(f)}; }

        template<typename F>
        auto transform(pipeline::Transform<F>&&)
        {
            assert(false && "Trying to wrap already Transformer in transform!");
        }

        template<typename F>
        auto transform(pipeline::Transform<F> const&)
        {
            assert(false && "Trying to wrap already Transformer in transform!");
        }
    }

    using pipeline::transform;
}