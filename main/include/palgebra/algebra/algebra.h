#pragma once
#include <palgebra/algebra/rules.h>

namespace agge
{
    namespace rules
    {
        // Sink
        template<typename S, typename E>
        S& operator<<(Sink<S>& s, Yield<E> const& e)
        {
            meld_tag<terminal>(e, s);
            return s._get_();
        }
    }
}