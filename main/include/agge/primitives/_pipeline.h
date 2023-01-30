#pragma once
#include <utility>

namespace agge
{
    namespace pipeline
    {
        template<typename E>
        struct terminal
        {
            E       &  _get_()       & { return           static_cast<E        &>(*this); }
            E const &  _get_() const & { return           static_cast<E const  &>(*this); }
            E      &&  _get_()      && { return std::move(static_cast<E       &&>(*this)); }
        };
    }
}