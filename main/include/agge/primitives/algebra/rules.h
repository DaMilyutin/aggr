#pragma once

#include <utility>
#include <type_traits>

#define FWD(a) std::forward<decltype(a)>(a)

namespace agge
{
    namespace rules
    {
        template<typename E>
        struct terminal
        {
            E       &  _get_()       & { return static_cast<E        &>(*this); }
            E const &  _get_() const & { return static_cast<E const  &>(*this); }
            E      &&  _get_()      && { return static_cast<E&&>(*this); }
        };

        template<typename E, bool closed = false>
        struct PointGenerator: terminal<PointGenerator<E, closed>>
        {
            //class const_iterator;
            //class const_sentinel;

            //const_iterator begin() const;
            //const_sentinel end() const;
        };

        template<typename E>
        struct Rasterizer: terminal<Rasterizer<E>> // Concept
        {};

        template<typename E>
        struct Decorator: terminal<Rasterizer<E>> // Concept
        {};
    }
}