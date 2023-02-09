#pragma once
#include <utility>
#define FWD(a) std::forward<decltype(a)>(a)

namespace plotting
{
    namespace pipeline
    {
        template<typename E>
        struct _terminal_
        {
            E& _get_()& { return static_cast<E&>(*this); }
            E const& _get_() const& { return static_cast<E const&>(*this); }
            E&& _get_()&& { return static_cast<E&&>(*this); }
        };

        template<typename E>
        struct Generator: public _terminal_<E>
        {};

        template<typename E>
        struct Chain: public _terminal_<E>
        {};

        template<typename E>
        struct Sink: public _terminal_<E>
        {};
    }
}