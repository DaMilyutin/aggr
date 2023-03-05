#pragma once
#include <palgebra/algebra/rules.h>

#include <palgebra/mediators/Path.h>

namespace agge
{
    template<typename S>
    struct Stored: public rules::Mediator<Stored<S>>
    {
        template<typename S>
        Stored(S&& s): stored(FWD(s)) {}

        S stored;
    };

    template<typename S>
    Stored<S&> stored(S& s) {
        return Stored<S&>(s);
    }

    template<typename S>
    Stored<S> stored(S&& s) {
        return Stored<S>(s);
    }

    template<typename S>
    Stored<S> stored() {
        return Stored<S>();
    }
}