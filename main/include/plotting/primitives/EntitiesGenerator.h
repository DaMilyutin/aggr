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

#include <plotting/generators/Generator.h>

namespace plotting
{
    template<typename GeneratorT, typename MakerT>
    class EntitiesGenerator: public pipeline::Generator<EntitiesGenerator<GeneratorT, MakerT>>
    {
    public:
        template<typename G, typename M>
        EntitiesGenerator(G&& g, M&& m): gen(std::forward<G>(g)), maker(std::forward<M>(m)) {}

        GeneratorT   gen;
        MakerT       maker;
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