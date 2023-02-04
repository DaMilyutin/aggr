#pragma once
#include <agge/primitives/pipeline.h>
#include <agge/color.h>

#include <plotting/primitives/Canvas.h>

namespace plotting
{
    template<typename E>
    struct Colored
    {
        E           entity;
        agge::color color;
    };

    template<typename E>
    struct Colored<E&>
    {
        E&          entity;
        agge::color color;
    };

    template<typename E>
    inline auto operator/(Colored<E>&, agge::color) = delete;
    template<typename E>
    inline auto operator/(Colored<E>&&, agge::color) = delete;


    template<typename E>
    inline Colored<E&> operator/(agge::pipeline::terminal<E>& e, agge::color c)
    {
        return {e._get_(), c};
    }

    template<typename E>
    inline Colored<E> operator/(agge::pipeline::terminal<E>&& e, agge::color c)
    {
        return {std::move(e._get_()), c};
    }

    template<typename S, typename Rn, typename Rs, typename E>
    inline Canvas<S, Rn, Rs>& operator<<(Canvas<S, Rn, Rs>& c, Colored<E>& e)
    {
        return c << e.entity << e.color;
    }

    template<typename S, typename Rn, typename Rs, typename E>
    inline Canvas<S, Rn, Rs>& operator<<(Canvas<S, Rn, Rs>& c, Colored<E>&& e)
    {
        return c << e.entity << e.color;
    }
}