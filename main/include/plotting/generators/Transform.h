#pragma once
#include <utility>
#include <plotting/generators/Generator,h>

namespace plotting
{
    namespace pipeline
    {
        template<typename Func>
        struct Transform: public agge::pipeline::terminal<Transform<Func>>
        {
            Func transform;
        };

        template<typename G, typename F>
        class TransformGenerator: public Generator<TransformGenerator<G, F>>
        {
            using UnderlyingIterator = std::remove_cv_t<decltype(std::begin(std::declval<G>()))>;
            using UnderlyingSentinel = std::remove_cv_t<decltype(std::end(std::declval<G>()))>;

            using from_t = std::remove_cvref_t<decltype(*std::begin(std::declval<G>()))>;
        public:
            using Sentinel = UnderlyingSentinel;

            class Iterator
            {
                friend class TransformGenerator;
                Iterator(TransformGenerator const& ref)
                    : _it(std::begin(ref.gen))
                    , ref(ref)
                {}
            public:
                Iterator& operator++() { ++_it; return *this; }
                auto const& operator*() const { return ref.transform(*it); }
                bool operator!=(Sentinel const& s) const { return _it != s; }
                bool operator==(Sentinel const& s) const { return _it == s; }
            private:
                UnderlyingIterator     _it;
                TransformGenerator const& ref;
            };

            Iterator begin() const { return Iterator(*this); }
            Sentinel end() const { return {}; }

            G     gen;
            F     transform;
        };

        template<typename G, typename F>
        TransformGenerator<G const&, F> operator/(Generator<G> const& g, Transform<F> const& f)
        {
            return {{}, g._get_(), f.select};
        }

        template<typename G, typename F>
        TransformGenerator<G const&, F> operator/(Generator<G> const& g, Transform<F>&& f)
        {
            return {{}, g._get_(), std::move(f.select)};
        }

        template<typename G, typename F>
        TransformGenerator<G, F> operator/(Generator<G>&& g, Transform<F>&& f)
        {
            return {{}, std::move(g._get_()), std::move(f.select)};
        }

        template<typename G, typename F>
        TransformGenerator<G, F> operator/(Generator<G, port_t>&& g, Transform<F> const& f)
        {
            return {{}, std::move(g._get_()), f.select};
        }
    }

    namespace transforms
    {
        template<typename F>
        auto transform(F const& f) { return pipeline::Transform<F const&>{ {}, f}; }

        template<typename F>
        auto transform(F&& f) { return pipeline::Transform<F>{ {}, std::move(f)}; }
    }
}