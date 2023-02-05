#pragma once
#include "CoordinateSystem.h"
#include <agge/color.h>
#include <agge/primitives/figures.h>
#include <agge/primitives/stroke.h>
#include <agge/primitives/stroke_features.h>

#include <agge/rendering/platform.h>
#include <agge/rendering/renderer.h>
#include <agge.text/font.h>

#include <plotting/primitives/Canvas.h>
#include <plotting/generators/LinesGen.h>
#include <plotting/generators/LabelsGen.h>

#include <plotting/primitives/Colored.h>

#include <functional>
#include <string>
#include <format>

namespace plotting
{
    struct Axes
    {
        struct LabelProperties
        {
            using Formatter = std::function<std::string(double)>;
            Formatter      format;
            TextProperties base{};
            agge::real_t   offset = 1.0f;
        };

        struct GridLineProperties
        {
            GridLineProperties()
            {
                line_style.set_cap(agge::caps::butt());
                line_style.set_join(agge::joins::bevel());
                line_style.width(1.);
                dash.remove_all_dashes();
                dash.add_dash(2.0f, 1.0f);
                dash.dash_start(1.0f);
                color = {128, 128, 128, 64};
            }
            agge::dash   dash;
            agge::stroke line_style;
            agge::color  color;
        };


        struct TickProperties
        {
            agge::real_t length = 7.0f;
            agge::real_t width  = 1.0f;
            agge::color  color = {128,128,128,128};
        };

        struct Separator
        {
            agge::color color = {128,128,128,128};
            float width = 2.;
        };

        struct AxisProperties
        {
            TickProperties     tick[3] = {{7.0f,1.5f}, {5.0f, 1.0f}, {2.0f,1.0f}}; // major middle, minor
            Separator          sep {};
            agge::real_t       tickGap{10.};
            int                tickSteps = 5;
            LabelProperties    labels {};
            GridLineProperties mutable grid;
            static inline AxisProperties from(agge::full_alignment al, agge::real_t offs = 0.0f)
            {
                auto text = TextProperties::from(al).color({200,200,200,255});
                return {{{7.0f,1.5f}, {5.0f, 1.0f}, {2.0f,1.0f}},
                        {}, 10., 5,
                        LabelProperties{{}, text, offs},
                        {}};
            }
        };

        port_area_t      position;
        CoordinateSystem coordinates;

        agge::color outer{40, 40, 40, 128};
        agge::color inner{20, 20, 20, 255};
        Separator   boundary{};


        agge::rect<AxisProperties> properties
            {AxisProperties::from({agge::align_center, agge::align_center}, 5.0f),
             AxisProperties::from({agge::align_near, agge::align_center}, 8.0f),
             AxisProperties::from({agge::align_center, agge::align_center}, 5.0f),
             AxisProperties::from({agge::align_far, agge::align_center}, 8.0f)};
    };

    namespace inner
    {
        inline double quantize_step(double step_repr)
        {
            int const sign = (step_repr>0) - (step_repr<0);
            step_repr = floor(log(fabs(step_repr))/log(10.));
            step_repr = sign*exp(step_repr*log(10.));
            return step_repr;
        }

        inline auto make_formatter(int digits)
        {
            return [digits](double x) { return std::format("{:+.{}f}", x, digits); };
        }

        inline double grow_step(float step, float min_resolution = 100.0f)
        {
            return ceil(min_resolution/fabs(step));
        }

        inline int digits(double inc)
        {
            double const ref = log(fabs(inc))/log(10.);
            if(ref >= 0.)
                return 1;
            int const d = int(floor(ref-0.1));
            return d < 0? -d: 1;
        }

        struct AxesArea
        {
            Axes const&   axes;
            agge::stroke& line_style;
        };

        using MajorLines = EntitiesGenerator<ParallelLinesGenerator<Selector_Any>, StylishLineMaker>;
        using GridLines = EntitiesGenerator<ParallelLinesGenerator<Selector_Any>,  FancyLineMaker>;
        using MinorLines = EntitiesGenerator<ParallelLinesGenerator<Selector_SkipOverPeriod>, StylishLineMaker>;
        using MajorLabels = EntitiesGenerator<ParallelLabelsGenerator, LabelMaker>;

        struct AxisXTicksMaker
        {
            AxisXTicksMaker(Axes const& axes, Axes::AxisProperties const& prop, agge::stroke& line_style)
                : axes(axes)
                , prop(prop)
                , line_style(line_style)
            {
                double step_repr = quantize_step((axes.coordinates.repr_area.x2 - axes.coordinates.repr_area.x1)/prop.tickSteps);
                agge::real_t const calcGap = fabs(axes.coordinates.port_area.x2 - axes.coordinates.port_area.x1)/prop.tickSteps;
                double const step_mul = grow_step(agge::real_t(step_repr*axes.coordinates.scale.x),
                                      std::max(prop.tickGap, calcGap));
                step_repr *= step_mul;
                double const mid_q = round(0.5*(axes.coordinates.repr_area.x2 + axes.coordinates.repr_area.x1)/step_repr)*step_repr;
                double const start_repr = mid_q - floor((mid_q - axes.coordinates.repr_area.x1)/step_repr)*step_repr;
                start = axes.coordinates.to_port_x(start_repr);
                step  = agge::real_t(axes.coordinates.scale.x*step_repr);
            }

            auto major_points(agge::real_t Y) const
            {
                auto const& pa = axes.coordinates.port_area;
                agge::real_t from = start;
                if(from < pa.x1 + 0.1f)
                    from += step;
                PointsOnSegmentGenerator<Selector_Any> points;
                points.initial = {from, Y};
                points.increment = {step, 0.0f};
                points.number = (int)ceil((pa.x2 - 0.1 - from)/step);
                return points;
            }

            auto medium_points(agge::real_t Y) const
            {
                auto const& pa = axes.coordinates.port_area;
                agge::real_t from = start - step*0.5f;
                if(from < pa.x1 + 0.1f)
                    from += step;
                PointsOnSegmentGenerator<Selector_Any> points;
                points.initial = {from, Y};
                points.increment = {step, 0.0f};
                points.number = (int)ceil((pa.x2 - 0.1 - from)/step);
                return points;
            }

            auto minor_points(agge::real_t Y) const
            {
                auto const& pa = axes.coordinates.port_area;
                agge::real_t const inc = step*0.1f;
                agge::real_t from = start - inc*9.0f;
                PointsOnSegmentGenerator<Selector_SkipOverPeriod> points;
                points.select.offset = 9;
                points.select.period = 5;
                while(from <= pa.x1 + 0.1f)
                    from += inc, --points.select.offset;
                points.select.offset = points.select.offset % points.select.period;
                points.initial = {from, Y};
                points.increment = {inc, 0.0f};
                points.number = (int)ceil((pa.x2 - 0.1 -from)/inc);
                return points;
            }

            auto major(agge::real_t Y, agge::real_t dir) const
            {
                line_style.width(prop.tick[0].width);
                return MajorLines{{}, {major_points(Y), {0, dir*prop.tick[0].length}},
                                  StylishLineMaker{line_style}}/prop.tick[0].color;
            }

            auto medium(agge::real_t Y, agge::real_t dir) const
            {
                line_style.width(prop.tick[1].width);
                return MajorLines{{}, {medium_points(Y), {0, dir*prop.tick[1].length}},
                                  StylishLineMaker{line_style}}/prop.tick[1].color;
            }

            auto minor(agge::real_t Y, agge::real_t dir) const
            {
                line_style.width(prop.tick[2].width);
                return MinorLines{{}, {minor_points(Y), {0, dir*prop.tick[2].length}}
                                 , StylishLineMaker{line_style}}/prop.tick[2].color;
            }

            auto grid(agge::real_t Y1, agge::real_t Y2) const
            {
                return GridLines{{}, {major_points(Y1), {0, Y2-Y1}},
                                 FancyLineMaker{prop.grid.dash, prop.grid.line_style}}/prop.grid.color;
            }

            auto labels(agge::real_t Y, agge::real_t dir) const
            {
                auto const& pa = axes.coordinates.port_area;
                agge::real_t from = start;
                if(from < pa.x1 + 0.1f)
                    from += step;
                double const initial = axes.coordinates.to_repr_x(from);
                double const inc = double(step)/axes.coordinates.scale.x;

                LabelMaker labelMaker{prop.labels.format
                                        ? prop.labels.format
                                        : make_formatter(digits(inc)),
                                      prop.labels.base};
                return MajorLabels{{}, {major_points(Y+prop.labels.offset*dir), initial, inc},
                                   labelMaker};
            }

            Axes                 const& axes;
            Axes::AxisProperties const& prop;
            agge::stroke&               line_style;
            agge::real_t                start;
            agge::real_t                step;
        };

        struct AxisYTicksMaker
        {
            AxisYTicksMaker(Axes const& axes, Axes::AxisProperties const& prop, agge::stroke& line_style)
                : axes(axes)
                , prop(prop)
                , line_style(line_style)
            {
                double step_repr = quantize_step((axes.coordinates.repr_area.y2 - axes.coordinates.repr_area.y1)/prop.tickSteps);
                agge::real_t const calcGap = fabs(axes.coordinates.port_area.y2 - axes.coordinates.port_area.y1)/prop.tickSteps;
                double const step_mul = grow_step(agge::real_t(step_repr*axes.coordinates.scale.y),
                                     std::max(prop.tickGap, calcGap));
                step_repr *= step_mul;
                double const mid_q = round(0.5*(axes.coordinates.repr_area.y2 + axes.coordinates.repr_area.y1)/step_repr)*step_repr;
                double const start_repr = mid_q - floor((mid_q - axes.coordinates.repr_area.y1)/step_repr)*step_repr;
                start = axes.coordinates.to_port_y(start_repr);
                step = agge::real_t(axes.coordinates.scale.y*step_repr);
            }

            auto major_points(agge::real_t X) const
            {
                auto const& pa = axes.coordinates.port_area;
                agge::real_t from = start;
                if(from < pa.y1 + 0.1f)
                    from += step;
                PointsOnSegmentGenerator<Selector_Any> points;
                points.initial = {X, from};
                points.increment = {0.0f, step};
                points.number = (int)ceil((pa.y2 - 0.1 - from)/step);
                return points;
            }

            auto medium_points(agge::real_t X) const
            {
                auto const& pa = axes.coordinates.port_area;
                agge::real_t from = start - step*0.5f;
                if(from < pa.y1 + 0.1f)
                    from += step;
                PointsOnSegmentGenerator<Selector_Any> points;
                points.initial = {X, from};
                points.increment = {0.0f, step};
                points.number = (int)ceil((pa.y2 - 0.1 - from)/step);
                return points;
            }

            auto minor_points(agge::real_t X) const
            {
                auto const& pa = axes.coordinates.port_area;
                agge::real_t const inc = step*0.1f;
                agge::real_t from = start - inc*9.0f;
                PointsOnSegmentGenerator<Selector_SkipOverPeriod> points;
                points.select.offset = 9;
                points.select.period = 5;
                while(from <= pa.y1 + 0.1f)
                    from += inc, --points.select.offset;
                points.select.offset = points.select.offset % points.select.period;
                points.initial = {X, from};
                points.increment = {0.0f, inc};
                points.number = (int)ceil((pa.y2 - 0.1 -from)/inc);
                return points;
            }


            auto major(agge::real_t X, agge::real_t dir) const
            {
                line_style.width(prop.tick[0].width);
                return MajorLines{{}, {major_points(X), {dir*prop.tick[0].length, 0}},
                                  StylishLineMaker{line_style}}/prop.tick[0].color;
            }

            auto medium(agge::real_t X, agge::real_t dir) const
            {
                line_style.width(prop.tick[1].width);
                return MajorLines{{}, {medium_points(X), {dir*prop.tick[1].length, 0}},
                                  StylishLineMaker{line_style}}/prop.tick[1].color;
            }

            auto minor(agge::real_t X, agge::real_t dir) const
            {
                line_style.width(prop.tick[2].width);
                return MinorLines{{}, {minor_points(X), {dir*prop.tick[2].length, 0}}
                                 , StylishLineMaker{line_style}}/prop.tick[2].color;
            }

            auto grid(agge::real_t X1, agge::real_t X2) const
            {
                return GridLines{{}, {major_points(X1), {X2-X1, 0}},
                                 FancyLineMaker{prop.grid.dash, prop.grid.line_style}}/prop.grid.color;
            }

            auto labels(agge::real_t X, agge::real_t dir) const
            {
                auto const& pa = axes.coordinates.port_area;
                agge::real_t from = start;
                if(from < pa.y1 + 0.1f)
                    from += step;
                double const initial = axes.coordinates.to_repr_y(from);
                double const inc = double(step)/axes.coordinates.scale.y;

                LabelMaker labelMaker{prop.labels.format
                                        ? prop.labels.format
                                        : make_formatter(digits(inc)),
                                      prop.labels.base};
                return MajorLabels{{}, {major_points(X+prop.labels.offset*dir), initial, inc},
                                   labelMaker};
            }

            Axes                 const& axes;
            Axes::AxisProperties const& prop;
            agge::stroke&               line_style;
            agge::real_t                start;
            agge::real_t                step;
        };
    }

    template<typename S, typename Rn, typename Rs>
    Canvas<S, Rn, Rs>& operator<<(Canvas<S, Rn, Rs>& c, inner::AxesArea axes_area)
    {
        auto const& axes = axes_area.axes;
        auto& line_style = axes_area.line_style;
        c.ras.reset_clipping();
        auto to_int = [](agge::rect_r const& r) { return agge::rect_i{int(round(r.x1)), int(round(r.y1)),
                                                                      int(round(r.x2)), int(round(r.y2))}; };
        agge::fill(c.surface, to_int(axes.position),
            agge::platform_blender_solid_color(axes.outer));
        c.ras.set_clipping(axes.position);

        agge::fill(c.surface, to_int(axes.coordinates.port_area),
            agge::platform_blender_solid_color(axes.inner));

        c << reset
            << agge::line(axes.coordinates.port_area.x1, axes.coordinates.port_area.y1,
                axes.coordinates.port_area.x1, axes.coordinates.port_area.y2)
            / line_style.width(axes.properties.x1.sep.width)
            << agge::platform_blender_solid_color(axes.properties.x1.sep.color);

        c << reset
            << agge::line(axes.coordinates.port_area.x2, axes.coordinates.port_area.y1,
                axes.coordinates.port_area.x2, axes.coordinates.port_area.y2)
            / line_style.width(axes.properties.x2.sep.width)
            << agge::platform_blender_solid_color(axes.properties.x2.sep.color);

        c << reset
            << agge::line(axes.coordinates.port_area.x1, axes.coordinates.port_area.y1,
                axes.coordinates.port_area.x2, axes.coordinates.port_area.y1)
            / line_style.width(axes.properties.y1.sep.width)
            << agge::platform_blender_solid_color(axes.properties.y1.sep.color);

        c << reset
            << agge::line(axes.coordinates.port_area.x1, axes.coordinates.port_area.y2,
                axes.coordinates.port_area.x2, axes.coordinates.port_area.y2)
            / line_style.width(axes.properties.y2.sep.width)
            << agge::platform_blender_solid_color(axes.properties.y2.sep.color);

        line_style.width(axes.boundary.width);
        c << reset
            << agge::line(axes.position.x1, axes.position.y1, axes.position.x1, axes.position.y2)/line_style
            << agge::line(axes.position.x1, axes.position.y2, axes.position.x2, axes.position.y2)/line_style
            << agge::line(axes.position.x2, axes.position.y2, axes.position.x2, axes.position.y1)/line_style
            << agge::line(axes.position.x2, axes.position.y1, axes.position.x1, axes.position.y1)/line_style
            << axes.boundary.color;

        c.ras.reset_clipping();
        return c;
    }

    template<typename S, typename Rn, typename Rs>
    Canvas<S, Rn, Rs>& operator<<(Canvas<S, Rn, Rs>& c, Axes const& axes)
    {
        agge::dash   dash;
        agge::stroke line_style;
        line_style.set_cap(agge::caps::butt());
        line_style.set_join(agge::joins::bevel());
        c << inner::AxesArea{axes, line_style};

        c.ras.set_clipping(axes.position);
        {
            inner::AxisXTicksMaker xTicks{axes, axes.properties.x1, line_style};
            c << xTicks.major(axes.coordinates.port_area.y1,-1);
            c << xTicks.medium(axes.coordinates.port_area.y1, -1);
            c << xTicks.minor(axes.coordinates.port_area.y1, -1);

            c << xTicks.labels(axes.coordinates.port_area.y1, -2.0f);

            c << xTicks.grid(axes.coordinates.port_area.y1, axes.coordinates.port_area.y2);
        }
        {
            inner::AxisXTicksMaker xTicks{axes, axes.properties.x2, line_style};
            c << xTicks.major(axes.coordinates.port_area.y2, 1);
            c << xTicks.medium(axes.coordinates.port_area.y2, 1);
            c << xTicks.minor(axes.coordinates.port_area.y2, 1);
            c << xTicks.labels(axes.coordinates.port_area.y2, 2.0f);

            //c << xTicks.grid(axes.coordinates.port_area.y1, axes.coordinates.port_area.y2);
        }
        {
            inner::AxisYTicksMaker yTicks{axes, axes.properties.y1, line_style};
            c << yTicks.major(axes.coordinates.port_area.x1, -1);
            c << yTicks.medium(axes.coordinates.port_area.x1, -1);
            c << yTicks.minor(axes.coordinates.port_area.x1, -1);

            c << yTicks.labels(axes.coordinates.port_area.x1, -1);

            c << yTicks.grid(axes.coordinates.port_area.x1, axes.coordinates.port_area.x2);
        }
        {
            inner::AxisYTicksMaker yTicks{axes, axes.properties.y2, line_style};
            c << yTicks.major(axes.coordinates.port_area.x2, 1);
            c << yTicks.medium(axes.coordinates.port_area.x2, 1);
            c << yTicks.minor(axes.coordinates.port_area.x2, 1);

            c << yTicks.labels(axes.coordinates.port_area.x2, 1.0f);
        }
        return c;
    }
}