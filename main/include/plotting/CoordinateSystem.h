#pragma once

#include <agge/types.h>

namespace plotting
{
    using port_t = agge::point<agge::real_t>;
    using repr_t = agge::point<double>;

    using repr_area_t = agge::rect<double>;
    using port_area_t = agge::rect<agge::real_t>;

    struct CoordinateSystem
    {
        CoordinateSystem() = default;
        CoordinateSystem(repr_area_t r)
            : repr_area{r}
        {}

        void update(port_area_t p)
        {
            port_area = p;
            scale.x = (double(port_area.x2)-port_area.x1)/(repr_area.x2-repr_area.x1);
            scale.y = (double(port_area.y2)-port_area.y1)/(repr_area.y2-repr_area.y1);
        }

        repr_area_t repr_area = repr_area_t{0., 1., 0., 1.};
        port_area_t port_area = port_area_t{agge::real_t(repr_area.x1), agge::real_t(repr_area.y1)
                                           ,agge::real_t(repr_area.x2), agge::real_t(repr_area.y2)};
        repr_t      scale = repr_t{1., 1.};

        port_t operator<<(repr_t const& f) const
        {
            return port_t{agge::real_t(port_area.x1 + scale.x*(f.x - repr_area.x1))
                         ,agge::real_t(port_area.y1 + scale.y*(f.y - repr_area.y1))};
        }
    };
}