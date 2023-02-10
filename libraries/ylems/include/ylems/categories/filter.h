#pragma once
#include <ylems/rules/abstract.h>
#include <limits>
#include <assert.h>

namespace ylems
{
    namespace elements
    {
        template<typename F, typename tag>
        struct Filter: rules::Link<F, tag>
        {
            template<typename Y>
            struct YieldDescriptor
            {
                using UnderlyingIterator = std::remove_cv_t<decltype(std::begin(std::declval<Y>()))>;
                using UnderlyingSentinel = std::remove_cv_t<decltype(std::end(std::declval<Y>()))>;
                using from_t = std::remove_cvref_t<decltype(*std::begin(std::declval<Y>()))>;

                struct Sentinel {};
                class Iterator
                {
                public:
                    Iterator(Y const& yield, F const& filter)
                        : _it(std::begin(yield))
                        , _end(std::end(yield))
                        , _select(filter)
                    {
                        next();
                    }

                    from_t const& operator*() const { return _cached; }
                    Iterator& operator++() { if(_it!=_end) ++_it; next(); return *this; }

                    bool operator!=(Sentinel) const { return _it != _end; }
                    bool operator==(Sentinel) const { return _it == _end; }
                private:
                    void next()
                    {
                        while(_it!=_end && !_select(_cached = *_it))
                            ++_it;
                    }

                    UnderlyingIterator         _it;
                    UnderlyingSentinel         _end;
                    F const&                   _select;
                    from_t           mutable   _cached{};
                };

                static auto begin(Y const& y, F const& f) { return Iterator(y, f); }
                static auto end(Y const&, F const&) { return Sentinel{}; }
            };

            template<typename Y> auto begin(Y const& y) const { return YieldDescriptor<Y>::begin(y, this->_get_()); }
            template<typename Y> auto end(Y const& y) const { return YieldDescriptor<Y>::end(y, this->_get_()); }
        };

        template<typename Func, typename tag>
        struct FilterWrap: public Filter<FilterWrap<Func, tag>, tag>
        {
            template<typename F>
            FilterWrap(F&& f) : select(std::forward<F>(f)) {}

            bool operator()(auto const& x) const { return select(x); }

            template<typename S, typename E>
            bool feed(S& sink, E&& e) const
            {
                if(!select(e))
                    return false;
                return (sink._get_()) << FWD(e);
            }

            Func select;
        };

        template<typename tag, typename F>
        auto filter(F const& f) { return FilterWrap<F const&, tag>{f}; }

        template<typename tag, typename F>
        auto filter(F&& f) { return FilterWrap<F, tag>{ std::move(f)}; }

        template<typename tag, typename F>
        auto filter(Filter<F, tag>&&)
        {
            assert(false && "Trying to wrap Filter in filter!");
        }

        template<typename tag, typename F>
        auto filter(Filter<F, tag> const&)
        {
            assert(false && "Trying to wrap Filter in filter!");
        }
    }
}