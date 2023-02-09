#pragma once
#include <plotting/generators/transform.h>
#include <plotting/generators/transform_or.h>

#include <plotting/types/point_types.h>

#include <agge/primitives/polyline.h>
#include <optional>

namespace plotting
{
    namespace transformers
    {
        struct ReprToPort: pipeline::Transform<ReprToPort>
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

            repr_t      scale = repr_t{1., 1.};
            repr_t      offset = repr_t{0., 0.};
        };

        struct PortToRepr: pipeline::Transform<PortToRepr>
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
                return {(offset.x + scale.x*t.x), (offset.y + scale.y*t.y)};
            }
            repr_diff_t operator()(port_diff_t const& t) const
            {
                return {(scale.x*t.dx), (scale.y*t.dy)};
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

        class ClipReprToPort: public pipeline::TransformOr<ClipReprToPort>
        {
            unsigned mutable cmd = agge::path_command_line_to;
            unsigned mutable next = agge::path_command_line_to;
            plotting::repr_area_t const& area;
            ReprToPort  const& transform;
        public:
            ClipReprToPort(plotting::repr_area_t const& area,
                                      ReprToPort const& transform)
                : area(area), transform(transform)
            {}

            std::optional<agge::polyline::Item> operator()(plotting::repr_t const& r) const
            {
                if(!in_area(area, r))
                {
                    cmd = next = agge::path_command_move_to;
                    return std::optional<agge::polyline::Item>();
                }
                cmd = next;
                next = agge::path_command_line_to;
                return agge::polyline::Item{transform(r), cmd};
            }
        };
    }
}