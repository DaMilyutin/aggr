#pragma once
#include <utility>
#define FWD(a) std::forward<decltype(a)>(a)

namespace ylems
{
    namespace rules
    {
        template<typename E, typename tag>
        struct _terminal_
        {
            E& _get_()& { return static_cast<E&>(*this); }
            E const& _get_() const& { return static_cast<E const&>(*this); }
            E&& _get_()&& { return static_cast<E&&>(*this); }
        };


        //TODO: concept/detect for Yield

        template<typename E, typename tag>
        struct Yield: public _terminal_<E, tag>
        {};

        template<typename E, typename tag>
        struct Link: public _terminal_<E, tag>
        {
            //for specific Link category(ex.Filter, Transform) one must provide methods
            //    template<typename Y> auto begin(Y const& y) const
            //    template<typename Y> auto end(Y const& y) const

            //    and method template<typename Sink, typename Elem> bool feed(Sink const&, Elem&&)

        };

        template<typename E, typename tag>
        struct Sink: public _terminal_<E, tag>
        {};
    }

}