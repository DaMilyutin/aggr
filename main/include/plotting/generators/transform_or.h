#pragma once
#include <plotting/generators/abstract.h>
#include <optional>

namespace plotting
{
    namespace pipeline
    {
        template<typename E>
        struct TransformOr: public Link<E>
        {
            template<typename Y>
            struct YieldDescriptor
            {
                using UnderlyingIterator = std::remove_cv_t<decltype(std::begin(std::declval<Y>()))>;
                using UnderlyingSentinel = std::remove_cv_t<decltype(std::end(std::declval<Y>()))>;

                using from_t = std::remove_cvref_t<decltype(*std::begin(std::declval<Y>()))>;
                using to_t = std::remove_cvref_t<decltype(std::declval<E>()(std::declval<from_t>()))>;
                using unwrapped_t = std::remove_cvref_t<decltype(*std::declval<to_t>())>;

                struct Sentinel {};

                class Iterator
                {
                public:
                    Iterator(Y const& y, E const& l)
                        : _it(std::begin(y))
                        , _end(std::end(y))
                        , _transform(l)
                    {
                        next();
                    }

                    unwrapped_t const& operator*() const { return *_cached; }
                    Iterator& operator++() { if(_it!=_end) ++_it; next(); return *this; }

                    bool operator!=(Sentinel) const { return _it != _end; }
                    bool operator==(Sentinel) const { return _it == _end; }
                private:
                    void next()
                    {
                        while(_it!=_end && !(_cached = _transform(*_it)))
                            ++_it;
                    }

                    UnderlyingIterator           _it;
                    UnderlyingSentinel           _end;
                    E                  const&    _transform;
                    to_t              mutable    _cached{};
                };

                static auto begin(Y const& y, E const& l) { return Iterator(y, l); }
                static auto end(Y const&, E const&) { return Sentinel{}; }
            };

            template<typename Y> auto begin(Y const& y) const { return YieldDescriptor<Y>::begin(y, this->_get_()); }
            template<typename Y> auto end(Y const& y) const { return YieldDescriptor<Y>::end(y, this->_get_()); }
        };

        template<typename Func>
        struct TransformOrWrap: public TransformOr<TransformOrWrap<Func>>
        {
            template<typename F>
            TransformOrWrap(F&& f): transform(FWD(f)) {}

            auto operator()(auto x) const { return transform(x); }
            Func   transform;
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