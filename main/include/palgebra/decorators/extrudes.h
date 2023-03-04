#pragma once
#include <palgebra/algebra/rules.h>
#include <agge/types/scalars.h>
#include <agge/types/Point.h>

#include <functional>

namespace agge
{
    namespace extrudes
    {
        struct Ortho: rules::Decorator<extrudes::Ortho>
        {
            Ortho() = default;
            Ortho(agge::real_t w): width(w) {}

            agge::real_t width = 1.0f;
        };

        struct OrthoWidthFun: rules::Decorator<extrudes::OrthoWidthFun>
        {
            OrthoWidthFun(std::function<double(double)> w): width(w) {}
            std::function<double(double)> width;
        };

        struct Directed: rules::Decorator<extrudes::Directed>
        {
            Directed() = default;
            Directed(agge::real_t w): width(w) {}

            agge::real_t   width = 1.0f;
            agge::Vector_r direction {0., 1};
        };

    }
}