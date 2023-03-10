#pragma once
#include <ylems/rules.h>
#include <ylems/categories.h>
#include <ylems/elements/yield.h>
#include <ylems/rules/algebra.h>

namespace agge
{
    namespace rules
    {
        template<typename E>
        struct terminal: ylems::rules::_terminal_<E> {};

        template<typename Y>    using Yield = ylems::rules::Yield<terminal, Y>;

        template<typename Y, typename L> using YieldLink = ylems::rules::YieldLink<terminal, Y, L>;
        template<typename L, typename S> using LinkSink = ylems::rules::LinkSink<terminal, L, S>;

        template<typename L>    using Link  = ylems::rules::Link<terminal, L>;
        template<typename S>    using Sink  = ylems::rules::Sink<terminal, S>;

        template<typename D>
        using Transform = ylems::categories::Transform<terminal, D>;

        template<typename D>
        struct Decorator: public Transform<D> {};

        YLEMS_MELD_OPERATION(terminal, operator/)

        YLEMS_MELD_RANGE_OPERATION(terminal, operator/)
    }
}

