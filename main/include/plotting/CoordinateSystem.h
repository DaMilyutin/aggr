#pragma once

#include <plotting/types/point_types.h>
#include <plotting/generators/transform.h>

namespace plotting
{
    struct ReprToPort: pipeline::Transformer<ReprToPort>
    {
        void from(repr_area_t const& repr_area, port_area_t const& port_area)
        {
            scale.x = (double(port_area.x2)-port_area.x1)/(repr_area.x2-repr_area.x1);
            scale.y = (double(port_area.y2)-port_area.y1)/(repr_area.y2-repr_area.y1);
            offset.x = port_area.x1 - scale.x*repr_area.x1;
            offset.y = port_area.y1 - scale.y*repr_area.y1;
        }

        port_t operator()(repr_t const& t) const
        {
            return {agge::real_t(offset.x + scale.x*t.x), agge::real_t(offset.y + scale.y*t.y)};
        }

        agge::real_t x(double v) const
        {
            return agge::real_t(offset.x + scale.x*v);
        }

        agge::real_t y(double v) const
        {
            return agge::real_t(offset.y + scale.y*v);
        }

        repr_t      scale  = repr_t{1., 1.};
        repr_t      offset = repr_t{0., 0.};
    };

    struct PortToRepr: pipeline::Transformer<PortToRepr>
    {
        void from(port_area_t const& port_area, repr_area_t const& repr_area)
        {
            scale.x = (repr_area.x2-repr_area.x1)/(double(port_area.x2)-port_area.x1);
            scale.y = (repr_area.y2-repr_area.y1)/(double(port_area.y2)-port_area.y1);
            offset.x = repr_area.x1 - scale.x*port_area.x1;
            offset.y = repr_area.y1 - scale.y*port_area.y1;
        }

        repr_t operator()(port_t const& t) const
        {
            return {agge::real_t(offset.x + scale.x*t.x), agge::real_t(offset.y + scale.y*t.y)};
        }

        double x(agge::real_t v) const
        {
            return offset.x + scale.x*v;
        }

        double y(agge::real_t v) const
        {
            return offset.y + scale.y*v;
        }

        repr_t      scale = repr_t{1., 1.};
        repr_t      offset = repr_t{0., 0.};
    };

    struct CoordinateSystem
    {
        CoordinateSystem() = default;
        CoordinateSystem(repr_area_t r)
            : repr_area{r}
        {}

        void update(port_area_t const& p)
        {
            port_area = p;
            repr2port.from(repr_area, port_area);
            port2repr.from(port_area, repr_area);
        }

        repr_area_t repr_area = repr_area_t{0., 1., 0., 1.};
        port_area_t port_area = port_area_t{agge::real_t(repr_area.x1), agge::real_t(repr_area.y1)
                                           ,agge::real_t(repr_area.x2), agge::real_t(repr_area.y2)};

        ReprToPort repr2port;
        PortToRepr port2repr;
    };

    inline port_t operator/(repr_t const& f, ReprToPort const& c)
    {
        return c(f);
    }
}