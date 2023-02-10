#pragma once
#include <utility>
#include <agge/color.h>
#include <agge/primitives/figures.h>
#include <agge/primitives/dash.h>
#include <agge/primitives/stroke.h>
#include <agge/primitives/stroke_features.h>

#include <agge/rendering/platform.h>
#include <agge/rendering/renderer.h>

#include <plotting/primitives/Canvas.h>

#include <plotting/piping.h>

namespace plotting
{
    template<typename S, typename Rn, typename Rs, typename G>
    plotting::Canvas<S, Rn, Rs>& operator<<(plotting::Canvas<S, Rn, Rs>& c,
                                            piping::Yield<G> const& entities)
    {
        c << reset;
        for(auto&& l : entities._get_())
            c << l;
        return c;
    }

    template<typename S, typename Rn, typename Rs, typename G>
    plotting::Canvas<S, Rn, Rs>& operator<<(plotting::Canvas<S, Rn, Rs>& c,
        piping::Yield<G>&& entities)
    {
        c << reset;
        for(auto&& l : entities._get_())
            c << l;
        return c;
    }
}