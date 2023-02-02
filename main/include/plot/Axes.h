#pragma once
#include "CoordinateSystem.h"
#include <agge/color.h>

namespace plotting
{
    struct Axes
    {
        port_area_t      position;
        CoordinateSystem coordinates;

        struct TickProperties
        {
            float length = 7;
            float width = 1;
            agge::color color = {255, 255, 255, 128};
        };

        struct Separator
        {
            agge::color color = {255, 255, 255, 128};
            float width = 2.;
        };

        struct AxisProperties
        {
            TickProperties tick[3] = {{10.0f,1.5f}, {5.0f, 1.0f}, {2.0f, 1.0f}}; // major middle, minor
            Separator      sep;
            int            tickSteps = 5;
        };

        agge::rect<AxisProperties> properties;
    };
}