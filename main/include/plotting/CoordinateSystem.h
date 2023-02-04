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
            return port_t{to_port_x(f.x), to_port_y(f.y)};
        }

        agge::real_t to_port_x(double x) const
        {
            return agge::real_t(port_area.x1 + scale.x*(x - repr_area.x1));
        }

        agge::real_t to_port_y(double y) const
        {
            return agge::real_t(port_area.y1 + scale.y*(y - repr_area.y1));
        }

        double to_repr_x(agge::real_t x) const
        {
            return repr_area.x1 + double(x - port_area.x1)/scale.x;
        }

        double to_repr_y(agge::real_t y) const
        {
            return repr_area.y1 + double(y - port_area.y1)/scale.y;
        }

    };
}