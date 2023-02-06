#pragma once

#include <agge/types.h>

namespace plotting
{
    using port_t = agge::point_r;
    using port_diff_t = agge::vector_r;
    using repr_t = agge::point<double>;
    using repr_diff_t = agge::agge_vector<double>;

    using repr_area_t = agge::rect<double>;
    using port_area_t = agge::rect<agge::real_t>;
}