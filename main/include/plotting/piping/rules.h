#pragma once
#include <ylems/rules.h>
#include <ylems/categories.h>
#include <ylems/elements/yield.h>

namespace plotting
{
    namespace piping
    {
        template<typename E>
        struct terminal: ylems::rules::_terminal_<E> {};

        template<typename Y>    using Yield = ylems::rules::Yield<Y, terminal>;
        template<typename L>    using Link  = ylems::rules::Link<L, terminal>;
        template<typename S>    using Sink  = ylems::rules::Sink<S, terminal>;

        YLEMS_MELD_OPERATION(terminal, operator/)

        YLEMS_MELD_RANGE_OPERATION(terminal, operator/)
    }
}

