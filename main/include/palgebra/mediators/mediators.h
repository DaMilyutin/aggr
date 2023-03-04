#pragma once
#include <palgebra/algebra/rules.h>

#include <palgebra/mediators/Path.h>

namespace agge
{
    template<typename Stored>
    struct Mediator: public rules::Mediator<agge::Mediator<Stored>>
    {
        template<typename S>
        Mediator(S&& s): stored(FWD(s)) {}

        Stored stored;
    };


    template<typename S>
    Mediator<S&> stored(S& s) {
        return Mediator<S&>(s);
    }

    template<typename S>
    Mediator<S> stored(S&& s) {
        return Mediator<S>(s);
    }

    template<typename S>
    Mediator<S> stored() {
        return Mediator<S>();
    }
}