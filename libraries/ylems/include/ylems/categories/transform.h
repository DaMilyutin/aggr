#pragma once
#include <ylems/rules/abstract.h>
#include <assert.h>

namespace ylems
{
    namespace elements
    {
        template<typename T, template<typename> typename tag>
        struct Transform: public rules::Link<T, tag>
        {
            template<typename Y>
            struct YieldDescriptor
            {
                using UnderlyingIterator = std::remove_all_extents_t<decltype(std::begin(std::declval<Y>()))>;
                using UnderlyingSentinel = std::remove_all_extents_t<decltype(std::end(std::declval<Y>()))>;
                using from_t = std::remove_all_extents_t<decltype(*std::begin(std::declval<Y>()))>;

                using Sentinel = std::remove_all_extents_t<decltype(std::end(std::declval<Y>()))>;
                class Iterator
                {
                    using UnderlyingIterator = std::remove_all_extents_t<decltype(std::begin(std::declval<Y>()))>;
                    using from_t = std::remove_cvref_t<decltype(*std::declval<UnderlyingIterator>())>;
                public:
                    Iterator(Y const& yield, T const& transform)
                        : _it(std::begin(yield))
                        , _transform(transform)
                    {}

                    Iterator& operator++() { ++_it; return *this; }
                    auto operator*() const { return _transform(*_it); }
                    bool operator!=(Sentinel const& s) const { return _it != s; }
                    bool operator==(Sentinel const& s) const { return _it == s; }
                private:
                    UnderlyingIterator  _it;
                    T const& _transform;
                };

                static auto begin(Y const& y, T const& l) { return Iterator(y, l); }
                static auto end(Y const& y, T const&) { return Sentinel{std::end(y)}; }
            };

            template<typename Y> auto begin(Y const& y) const { return YieldDescriptor<Y>::begin(y, this->_get_()); }
            template<typename Y> auto end(Y const& y) const { return YieldDescriptor<Y>::end(y, this->_get_()); }
        };

        template<typename Func, template<typename> typename tag>
        struct TransformWrap: public Transform<TransformWrap<Func, tag>, tag>
        {
            template<typename F>
            TransformWrap(F&& f): transform(FWD(f)) {}

            TransformWrap(TransformWrap&&) = default;
            TransformWrap(TransformWrap const&) = default;

            auto operator()(auto x) const { return transform(x); }

            template<typename S, typename E>
            bool feed(S& sink, E&& e) const
            {
                return sink.eat(transform(FWD(e)));
            }

            Func transform;
        };

        template<template<typename> typename tag, typename F>
        auto transform(F const& f) { return TransformWrap<F const&, tag>{ f}; }

        template<template<typename> typename tag, typename F>
        auto transform(F&& f) { return TransformWrap<F, tag>{FWD(f)}; }

        template<template<typename> typename tag, typename F>
        auto transform(Transform<F, tag>&&)
        {
            assert(false && "Trying to wrap already Transformer in transform!");
        }

        template<template<typename> typename tag, typename F>
        auto transform(Transform<F, tag> const&)
        {
            assert(false && "Trying to wrap already Transformer in transform!");
        }
    }
}