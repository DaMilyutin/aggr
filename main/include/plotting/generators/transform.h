#pragma once
#include <utility>
#include <assert.h>

#include <plotting/generators/generator.h>

namespace plotting
{
    namespace pipeline
    {
        template<typename E>
        struct Transformer: public agge::pipeline::terminal<E> {};

        template<typename Func>
        struct Transform: public Transformer<Transform<Func>>
        {
            template<typename F>
            Transform(F&& f): transform(std::forward<F>(f)) {}

            auto operator()(auto x) const { return transform(x); }
            Func mutable transform;
        };

        template<typename G, typename F>
        class TransformGenerator: public Generator<TransformGenerator<G, F>>
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
        TransformGenerator<G const&, F> operator/(Generator<G> const& g, Transformer<F> const& f)
        {
            return {g._get_(), f._get_()};
        }

        template<typename G, typename F>
        TransformGenerator<G const&, F> operator/(Generator<G> const& g, Transformer<F>&& f)
        {
            return {g._get_(), std::move(f)._get_()};
        }

        template<typename G, typename F>
        TransformGenerator<G, F> operator/(Generator<G>&& g, Transformer<F>&& f)
        {
            return {std::move(g)._get_(), std::move(f)._get_()};
        }

        template<typename G, typename F>
        TransformGenerator<G, F> operator/(Generator<G>&& g, Transformer<F> const& f)
        {
            return {std::move(g)._get_(), f._get_()};
        }

        template<typename F>
        auto transform(F const& f) { return pipeline::Transform<F const&>{ f}; }

        template<typename F>
        auto transform(F&& f) { return pipeline::Transform<F>{std::move(f)}; }

        template<typename F>
        auto transform(pipeline::Transformer<F>&&)
        {
            assert(false && "Trying to wrap already Transformer in transform!");
        }

        template<typename F>
        auto transform(pipeline::Transformer<F> const&)
        {
            assert(false && "Trying to wrap already Transformer in transform!");
        }
    }

    using pipeline::transform;
}