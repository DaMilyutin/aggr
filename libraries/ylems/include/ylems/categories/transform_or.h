#pragma once
#include <ylems/rules/abstract.h>
#include <optional>

namespace ylems
{
    namespace elements
    {
        template<typename E, template<typename> typename tag>
        struct TransformOr: public rules::Link<E, tag>
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

        template<typename Func, template<typename> typename tag>
        struct TransformOrWrap: public TransformOr<TransformOrWrap<Func, tag>, tag>
        {
            template<typename F>
            TransformOrWrap(F&& f): transform(FWD(f)) {}

            auto operator()(auto x) const { return transform(x); }
            Func  transform;
        };

        template<typename Sel, typename Trans, template<typename> typename tag>
        struct TransformOrWrap2: public TransformOr<TransformOrWrap2<Sel, Trans, tag>, tag>
        {
            template<typename F, typename G>
            TransformOrWrap2(F&& f, G&& g): select(FWD(f)), trans(FWD(g)) {}

            auto operator()(auto x) const { return select(x)? std::make_optional(trans(x)): std::nullopt; }
            Sel   select;
            Trans trans;
        };

        template<template<typename> typename tag, typename F>
        auto transform_or(F const& f) { return TransformOrWrap<F const&, tag>{ f}; }

        template<template<typename> typename tag, typename F>
        auto transform_or(F&& f) { return TransformOrWrap<F, tag>{std::move(f)}; }

        template<template<typename> typename tag, typename F, typename G>
        auto transform_or(F&& f, G&& g) { return TransformOrWrap2<F, G, tag>{FWD(f), FWD(g)}; }

        template<template<typename> typename tag, typename F>
        auto transform_or(TransformOr<F, tag>&&)
        {
            assert(false && "Trying to wrap already Transformer in transform!");
        }

        template<template<typename> typename tag, typename F>
        auto transform_or(TransformOr<F, tag> const&)
        {
            assert(false && "Trying to wrap already Transformer in transform!");
        }
    }
}