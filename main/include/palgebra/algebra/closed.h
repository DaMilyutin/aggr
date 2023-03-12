#pragma once

#include <palgebra/algebra/algebra.h>
#include <palgebra/algebra/start.h>
#include <palgebra/elements/Vertex.h>

namespace agge
{
    namespace rules
    {
        template<typename P>
        auto closed(Yield<P>&& p)
        {
            return cycle<1>(FWD(p));
        }

        template<typename P>
        auto closed(Yield<P> const& p)
        {
            return cycle<1>(p);
        }
    }
}
