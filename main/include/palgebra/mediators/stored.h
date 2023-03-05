#pragma once
#include <palgebra/algebra/rules.h>

#include <palgebra/mediators/Path.h>

namespace agge
{
    template<typename Stored>
    struct Stored: public rules::Mediator<agge::Stored<Stored>>
    {
        template<typename S>
        Stored(S&& s): stored(FWD(s)) {}

        Stored stored;
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