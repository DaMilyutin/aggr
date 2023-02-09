#pragma once
#include <utility>
#define FWD(a) std::forward<decltype(a)>(a)

namespace plotting
{
    namespace pipeline
    {
        template<typename E, typename tag = void>
        struct _terminal_
        {
            E& _get_()& { return static_cast<E&>(*this); }
            E const& _get_() const& { return static_cast<E const&>(*this); }
            E&& _get_()&& { return static_cast<E&&>(*this); }
        };

        template<typename E, typename tag = void>
        struct Yield: public _terminal_<E, tag>
        {};

        template<typename E, typename tag = void>
        struct Link: public _terminal_<E, tag>
        {
            // for specific Link category (ex. Filter, Transform) one must provide methods
            // template<typename Y> auto begin(Y const& y) const
            // template<typename Y> auto end(Y const& y) const
        };


        template<typename E, typename tag = void>
        struct Sink: public _terminal_<E, tag>
        {};
    }
}