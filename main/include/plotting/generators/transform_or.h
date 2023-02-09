#pragma once
#include <plotting/generators/abstract.h>
#include <optional>

namespace plotting
{
    namespace pipeline
    {
        template<typename E>
        struct TransformOr: public Chain<E> {};

        template<typename Func>
        struct TransformOrWrap: public TransformOr<TransformOrWrap<Func>>
        {
            template<typename F>
            TransformOrWrap(F&& f): transform(std::forward<F>(f)) {}

            auto operator()(auto x) const { return transform(x); }
            Func mutable transform;
        };

        template<typename Sel, typename Trans>
        struct TransformOrWrap2: public TransformOr<TransformOrWrap2<Sel, Trans>>
        {
            template<typename F, typename G>
            TransformOrWrap2(F&& f, G&& g): select(FWD(f)), trans(FWD(g)) {}

            auto operator()(auto x) const { return select(x)? std::make_optional(trans(x)): std::nullopt; }
            Sel   mutable select;
            Trans mutable trans;
        };

        template<typename G, typename F>
        class TransformOrGenerator: public Generator<TransformOrGenerator<G, F>>
        {
            using UnderlyingIterator = std::remove_cv_t<decltype(std::begin(std::declval<G>()))>;
            using UnderlyingSentinel = std::remove_cv_t<decltype(std::end(std::declval<G>()))>;

            using from_t = std::remove_cvref_t<decltype(*std::begin(std::declval<G>()))>;
            using to_t = std::remove_cvref_t<decltype(std::declval<F>()(std::declval<from_t>()))>;
            using unwrapped_t = std::remove_cvref_t<decltype(*std::declval<to_t>())>;
        public:
            struct Sentinel {};

            template<typename G, typename T>
            TransformOrGenerator(G&& g, T&& t): gen(std::forward<G>(g)), transform(std::forward<T>(t)) {}

            class Iterator
            {
                friend class TransformOrGenerator;
                Iterator(TransformOrGenerator const& ref)
                    : _it(std::begin(ref.gen))
                    , _end(std::end(ref.gen))
                    , ref(ref)
                {
                    next();
                }
            public:
                unwrapped_t const& operator*() const { return *cached; }
                Iterator& operator++() { if(_it!=_end) ++_it; next(); return *this; }

                bool operator!=(Sentinel) const { return _it != _end; }
                bool operator==(Sentinel) const { return _it == _end; }
            private:
                void next()
                {
                    while(_it!=_end && !(cached = ref.transform(*_it)))
                        ++_it;
                }

                UnderlyingIterator           _it;
                UnderlyingSentinel           _end;
                TransformOrGenerator const&   ref;
                to_t              mutable     cached{};
            };

            Iterator begin() const { return Iterator(*this); }
            Sentinel end() const { return {}; }

            G     gen;
            F     transform;
        };

        template<typename G, typename F>
        TransformOrGenerator<G const&, F> operator/(Generator<G> const& g, TransformOr<F> const& f)
        {
            return {g._get_(), f._get_()};
        }

        template<typename G, typename F>
        TransformOrGenerator<G const&, F> operator/(Generator<G> const& g, TransformOr<F>&& f)
        {
            return {g._get_(), std::move(f)._get_()};
        }

        template<typename G, typename F>
        TransformOrGenerator<G, F> operator/(Generator<G>&& g, TransformOr<F>&& f)
        {
            return {std::move(g)._get_(), std::move(f)._get_()};
        }

        template<typename G, typename F>
        TransformOrGenerator<G, F> operator/(Generator<G>&& g, TransformOr<F> const& f)
        {
            return {std::move(g)._get_(), f._get_()};
        }

        template<typename F>
        auto transform_or(F const& f) { return pipeline::TransformOrWrap<F const&>{ f}; }

        template<typename F>
        auto transform_or(F&& f) { return pipeline::TransformOrWrap<F>{std::move(f)}; }

        template<typename F, typename G>
        auto transform_or(F&& f, G&& g) { return pipeline::TransformOrWrap2<F, G>{FWD(f), FWD(g)}; }

        template<typename F>
        auto transform_or(pipeline::TransformOr<F>&&)
        {
            assert(false && "Trying to wrap already Transformer in transform!");
        }

        template<typename F>
        auto transform_or(pipeline::TransformOr<F> const&)
        {
            assert(false && "Trying to wrap already Transformer in transform!");
        }
    }

    using pipeline::transform_or;
}