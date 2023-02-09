#pragma once
#include <agge/primitives/pipeline.h>
#include <utility>
#define FWD(a) std::forward<decltype(a)>(a)

namespace plotting
{
    namespace pipeline
    {
        template<typename E>
        struct Generator: public agge::pipeline::terminal<E>
        {};

        template<typename E>
        struct Chain: public agge::pipeline::terminal<E>
        {};

        template<typename E>
        struct Sink: public agge::pipeline::terminal<E>
        {};
    }
}