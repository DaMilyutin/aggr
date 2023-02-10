#pragma once
#include <ylems/rules.h>
#include <ylems/categories.h>

namespace plotting
{
    namespace piping
    {
        struct Plotting_tag {};

        template<typename Y>    using Yield = ylems::rules::Yield<Y, Plotting_tag>;
        template<typename L>    using Link  = ylems::rules::Link<L, Plotting_tag>;
        template<typename S>    using Sink  = ylems::rules::Sink<S, Plotting_tag>;
    }

    YLEMS_MELD_OPERATION(piping::Plotting_tag, operator/)
}

