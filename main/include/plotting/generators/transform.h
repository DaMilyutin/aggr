#pragma once
#include <plotting/generators/abstract.h>
#include <assert.h>

namespace plotting
{
    namespace pipeline
    {
        template<typename T>
        struct Transform: public Link<T>
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

        template<typename Func>
        struct TransformWrap: public Transform<TransformWrap<Func>>
        {
            template<typename F>
            TransformWrap(F&& f): transform(FWD(f)) {}

            TransformWrap(TransformWrap&&) = default;
            TransformWrap(TransformWrap const&) = default;

            auto operator()(auto x) const { return transform(x); }

            template<typename S, typename E>
            bool feed(Sink<S>& sink, E&& e) const
            {
                return sink._get_().eat(transform(FWD(e)));
            }

            Func transform;
        };

        template<typename F>
        auto transform(F const& f) { return pipeline::TransformWrap<F const&>{ f}; }

        template<typename F>
        auto transform(F&& f) { return pipeline::TransformWrap<F>{FWD(f)}; }

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