#pragma once
#include <palgebra/algebra/rules.h>

namespace agge
{
    namespace caps
    {
        struct Butt: rules::Decorator<caps::Butt>
        {};

        struct Round: rules::Decorator<caps::Round>
        {};
    }
}