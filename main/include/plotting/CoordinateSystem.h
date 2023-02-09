#pragma once

#include <plotting/types/point_types.h>
#include <plotting/chain-helpers/transformers.h>

namespace plotting
{
    struct CoordinateSystem
    {
        CoordinateSystem() = default;
        CoordinateSystem(repr_area_t r)
            : repr_area{r}
        {}

        void update(repr_area_t const& p)
        {
            repr_area = p;
            repr2port.from(repr_area, port_area);
            port2repr.from(port_area, repr_area);
        }

        void update(port_area_t const& p)
        {
            port_area = p;
            repr2port.from(repr_area, port_area);
            port2repr.from(port_area, repr_area);
        }

        repr_area_t repr_area = repr_area_t{0., 1., 0., 1.};
        port_area_t port_area = port_area_t{agge::real_t(repr_area.x1), agge::real_t(repr_area.y1)
                                           ,agge::real_t(repr_area.x2), agge::real_t(repr_area.y2)};

        transformers::ReprToPort repr2port;
        transformers::PortToRepr port2repr;
    };

    inline auto clip_repr2port(CoordinateSystem const& c)
    {
        return transformers::ClipReprToPort(c.repr_area, c.repr2port);
    }

    inline void shift(repr_area_t& a, repr_diff_t const& v)
    {
        a.x1 += v.dx; a.x2 += v.dx;
        a.y1 += v.dy; a.y2 += v.dy;
    }

    inline void zoom(repr_area_t& a, repr_t const& p, double f)
    {
        a.x1 = p.x + f*(a.x1 - p.x); a.x2 = p.x + f*(a.x2 - p.x);
        a.y1 = p.y + f*(a.y1 - p.y); a.y2 = p.y + f*(a.y2 - p.y);
    }
}