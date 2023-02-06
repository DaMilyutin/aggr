#pragma once
#include <agge/primitives/pipeline.h>

namespace plotting
{
    namespace pipeline
    {
        template<typename E>
        struct Generator: public agge::pipeline::terminal<E>
        {};
    }
}