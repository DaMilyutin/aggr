#pragma once

#include <agge/types.h>
#include <agge/utils/math.h>

namespace plotting
{
    using port_t = agge::point_r;
    using port_diff_t = agge::vector_r;
    using repr_t = agge::point<double>;
    using repr_diff_t = agge::agge_vector<double>;

    using repr_area_t = agge::rect<double>;
    using port_area_t = agge::rect<agge::real_t>;

    inline bool in_area(plotting::repr_area_t const& a, plotting::repr_t const& p)
    {
        return (a.x1 < p.x&& p.x < a.x2) && (a.y2 < p.y&& p.y < a.y1);
    }

    inline bool in_area(agge::rect_r const& a, agge::point_r const& p)
    {
        return (a.x1 < p.x&& p.x < a.x2) && (a.y1 < p.y&& p.y < a.y2);
    }
}