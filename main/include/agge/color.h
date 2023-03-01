#pragma once

#include "types.h"

#include <math.h>

namespace agge
{
    struct color
    {
        uint8_t r, g, b, a;
        static constexpr uint8_t u255 = uint8_t(255);

        color inverted() const { return {uint8_t(255-r), uint8_t(255-g), uint8_t(255-b), a}; }

        static color make(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 0xFF);
    };

    inline color interp(color const& x, color const& y, double t)
    {
        return color{ (uint8_t) round(x.r + (y.r - x.r)*t),
                      (uint8_t) round(x.g + (y.g - x.g)*t),
                      (uint8_t) round(x.b + (y.b - x.b)*t),
                      (uint8_t) round(x.a + (y.a - x.a)*t)};
    }
}
