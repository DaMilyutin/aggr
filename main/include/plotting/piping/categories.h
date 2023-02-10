#pragma once
#include "rules.h"
#include <ylems/categories.h>

namespace plotting
{
    namespace piping
    {
        template<typename T> using Transform   = ylems::categories::Transform<T, terminal>;
        template<typename T> using TransformOr = ylems::categories::TransformOr<T, terminal>;
        template<typename T> using Filter      = ylems::categories::Filter<T, terminal>;
    }
}