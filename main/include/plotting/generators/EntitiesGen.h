#pragma once
#include <agge/color.h>
#include <agge/primitives/figures.h>
#include <agge/primitives/dash.h>
#include <agge/primitives/stroke.h>
#include <agge/primitives/stroke_features.h>

#include <agge/rendering/platform.h>
#include <agge/rendering/renderer.h>

#include <plotting/primitives/Canvas.h>

namespace plotting
{
    struct Selector_Any
    {
        bool operator()(int) const { return true; }
    };

    struct Selector_SkipOverPeriod
    {
        int period = 10;
        int offset = 0;

        bool operator()(int x) const
        {
            return (x-offset)%period != 0;
        }
    };

    template<typename Generator, typename Maker>
    class EntitiesGenerator: public agge::pipeline::terminal<EntitiesGenerator<Generator, Maker>>
    {
    public:
        Generator   gen;
        Maker       maker;
    };

    template<typename S, typename Rn, typename Rs, typename Generator, typename Maker>
    plotting::Canvas<S, Rn, Rs>& operator<<(plotting::Canvas<S, Rn, Rs>& c,
        EntitiesGenerator<Generator, Maker> const& entities)
    {
        c << reset;
        for(auto&& l : entities.gen)
            c << entities.maker(l);
        return c;
    }
}