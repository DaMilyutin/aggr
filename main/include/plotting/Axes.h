#pragma once
#include "CoordinateSystem.h"
#include <agge/color.h>
#include <agge/primitives/figures.h>
#include <agge/primitives/stroke.h>
#include <agge/primitives/stroke_features.h>

#include <agge/rendering/platform.h>
#include <agge/rendering/renderer.h>
#include <agge.text/font.h>

#include <plotting/Canvas.h>
#include <plotting/LinesGen.h>
#include <plotting/LabelsGen.h>

#include <functional>
#include <string>
#include <format>

namespace plotting
{
    struct Axes
    {
        port_area_t      position;
        CoordinateSystem coordinates;

        agge::color outer { 40, 40, 40, 128};
        agge::color inner { 20, 20, 20, 255};

        struct LabelProperties
        {
            using Formatter = std::function<std::string(double)>;
            Formatter      format;
            TextProperties base{};
            agge::real_t   offset = 1.0f;
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
            TickProperties  tick[3] = {{7.0f,1.5f}, {5.0f, 1.0f}, {2.0f,1.0f}}; // major middle, minor
            Separator       sep {};
            int             tickSteps = 5;
            LabelProperties labels {};

            static inline AxisProperties from(agge::full_alignment al, agge::real_t offs = 0.0f)
            {
                auto text = TextProperties::from(al).color({200,200,200,255});
                return {{{7.0f,1.5f}, {5.0f, 1.0f}, {2.0f,1.0f}},
                        {}, 5,
                        LabelProperties{{}, text, offs}};
            }
        };

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
            return [digits](double x) { return std::format("{:.{}f}", x, digits); };
        }

        inline double grow_step(float step, float min_resolution = 100.0f)
        {
            return ceil(min_resolution/fabs(step));
        }

        inline int digits(double inc)
        {
            int d = int(ceil(log(fabs(inc))/log(10.) + 0.1));
            if(d > 1) d = 1;
            return d;
        }

        struct AxesArea
        {
            Axes const&   axes;
            agge::stroke& line_style;
        };

        using MajorLines = EntitiesGenerator<ParallelLinesGenerator<Selector_Any>, StylishLineMaker>;
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
                double const step_mul = grow_step(agge::real_t(step_repr*axes.coordinates.scale.x));
                step_repr *= step_mul;
                double const mid_q = round(0.5*(axes.coordinates.repr_area.x2 + axes.coordinates.repr_area.x1)/step_repr)*step_repr;
                double const start_repr = mid_q - floor((mid_q - axes.coordinates.repr_area.x1)/step_repr)*step_repr;
                start = axes.coordinates.to_port_x(start_repr);
                step  = agge::real_t(axes.coordinates.scale.x*step_repr);
            }

            MajorLines major(agge::real_t Y, agge::real_t dir) const
            {
                line_style.width(prop.tick[0].width);
                auto const& pa = axes.coordinates.port_area;
                agge::real_t from = start;
                if(from < pa.x1 + 0.1f)
                    from += step;
                ParallelLinesGenerator<Selector_Any> gen;
                gen.initial.start = {from, Y};
                gen.initial.end = {from, Y+dir*prop.tick[0].length};
                gen.direction = {step, 0.0f};
                gen.number = (int)ceil((pa.x2 - 0.1 - from)/step);
                return {gen, StylishLineMaker{line_style}, prop.tick[0].color};
            }

            MajorLabels labels(agge::real_t Y, agge::real_t dir) const
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
                //agge::box_r box = estimate_box(labelMaker, initial);

                ParallelLabelsGenerator gen;
                gen.initial.position = {from, Y+dir*prop.labels.offset};
                gen.initial.value = initial;
                gen.inc = inc;
                gen.direction = {step, 0.0f};
                gen.number = (int)ceil((pa.x2 - 0.1 - from)/step);

                return {gen, labelMaker,
                        prop.labels.base._fill};
            }

            MajorLines medium(agge::real_t Y, agge::real_t dir) const
            {
                line_style.width(prop.tick[1].width);
                auto const& pa = axes.coordinates.port_area;
                agge::real_t from = start-step*0.5f;
                if(from < pa.x1 + 0.1f)
                    from += step;
                ParallelLinesGenerator<Selector_Any> gen;
                gen.initial.start = {from, Y};
                gen.initial.end = {from, Y+dir*prop.tick[1].length};
                gen.direction = {step, 0.0f};
                gen.number = (int)ceil((pa.x2 - 0.1 - from)/step);
                return {gen, StylishLineMaker{line_style}, prop.tick[1].color};
            }

            MinorLines minor(agge::real_t Y, agge::real_t dir) const
            {
                line_style.width(prop.tick[2].width);
                auto const& pa = axes.coordinates.port_area;
                agge::real_t const inc = step*0.1f;
                agge::real_t from = start - inc*9.0f;
                ParallelLinesGenerator<Selector_SkipOverPeriod> gen;
                gen.select.offset = 9;
                gen.select.period = 5;
                while(from <= pa.x1 + 0.1f)
                    from += inc, --gen.select.offset;
                gen.select.offset = gen.select.offset % gen.select.period;
                gen.initial.start = {from, Y};
                gen.initial.end = {from, Y+dir*prop.tick[2].length};
                gen.direction = {inc, 0.0f};
                gen.number = (int)ceil((pa.x2 - 0.1 -from)/inc);
                return {gen, StylishLineMaker{line_style}, prop.tick[2].color};
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
                double const step_mul = grow_step(agge::real_t(step_repr*axes.coordinates.scale.y));
                step_repr *= step_mul;
                double const mid_q = round(0.5*(axes.coordinates.repr_area.y2 + axes.coordinates.repr_area.y1)/step_repr)*step_repr;
                double const start_repr = mid_q - floor((mid_q - axes.coordinates.repr_area.y1)/step_repr)*step_repr;
                start = axes.coordinates.to_port_y(start_repr);
                step = agge::real_t(axes.coordinates.scale.y*step_repr);
            }

            MajorLines major(agge::real_t X, agge::real_t dir) const
            {
                line_style.width(prop.tick[0].width);
                auto const& pa = axes.coordinates.port_area;
                agge::real_t from = start;
                if(from < pa.y1 + 0.1f)
                    from += step;
                ParallelLinesGenerator<Selector_Any> gen;
                gen.initial.start = {X, from};
                gen.initial.end = {X+dir*prop.tick[0].length, from};
                gen.direction = {0.0f, step};
                gen.number = (int)ceil((pa.y2  - 0.1 - from)/step);
                return {gen, StylishLineMaker{line_style}, prop.tick[0].color};
            }

            MajorLabels labels(agge::real_t X, agge::real_t dir) const
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
                //agge::box_r box = estimate_box(labelMaker, initial);

                ParallelLabelsGenerator gen;
                gen.initial.position = {X+dir*prop.labels.offset, from};
                gen.initial.value = initial;
                gen.inc = inc;
                gen.direction = {0.0f, step};
                gen.number = (int)ceil((pa.y2 - 0.1 - from)/step);

                return {gen, labelMaker,
                        prop.labels.base._fill};
            }

            MajorLines medium(agge::real_t X, agge::real_t dir) const
            {
                line_style.width(prop.tick[1].width);
                auto const& pa = axes.coordinates.port_area;
                agge::real_t from = start-step*0.5f;
                if(from < pa.y1 + 0.1f)
                    from += step;
                ParallelLinesGenerator<Selector_Any> gen;
                gen.initial.start = {X, from};
                gen.initial.end = {X+dir*prop.tick[1].length, from};
                gen.direction = {0.0f, step};
                gen.number = (int)ceil((pa.y2 - 0.1 - from)/step);
                return {gen, StylishLineMaker{line_style}, prop.tick[1].color};
            }

            MinorLines minor(agge::real_t X, agge::real_t dir) const
            {
                line_style.width(prop.tick[2].width);
                auto const& pa = axes.coordinates.port_area;
                agge::real_t const inc = step*0.1f;
                agge::real_t from = start - inc*9.0f;
                ParallelLinesGenerator<Selector_SkipOverPeriod> gen;
                gen.select.offset = 9;
                gen.select.period = 5;
                while(from <= pa.y1 + 0.1f)
                    from += inc, --gen.select.offset;
                gen.select.offset = gen.select.offset % gen.select.period;
                gen.initial.start = {X, from};
                gen.initial.end = {X+dir*prop.tick[2].length, from};
                gen.direction = {0.0f, inc};
                gen.number = (int)ceil((pa.y2 - 0.1 -from)/inc);
                return {gen, StylishLineMaker{line_style}, prop.tick[2].color};
            }

            Axes                 const& axes;
            Axes::AxisProperties const& prop;
            agge::stroke& line_style;
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

        c.ras.reset_clipping();
        return c;
    }

    template<typename S, typename Rn, typename Rs>
    Canvas<S, Rn, Rs>& operator<<(Canvas<S, Rn, Rs>& c, Axes const& axes)
    {
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
        }
        {
            inner::AxisXTicksMaker xTicks{axes, axes.properties.x2, line_style};
            c << xTicks.major(axes.coordinates.port_area.y2, 1);
            c << xTicks.medium(axes.coordinates.port_area.y2, 1);
            c << xTicks.minor(axes.coordinates.port_area.y2, 1);
            c << xTicks.labels(axes.coordinates.port_area.y2, 2.0f);
        }
        {
            inner::AxisYTicksMaker yTicks{axes, axes.properties.y1, line_style};
            c << yTicks.major(axes.coordinates.port_area.x1, -1);
            c << yTicks.medium(axes.coordinates.port_area.x1, -1);
            c << yTicks.minor(axes.coordinates.port_area.x1, -1);

            c << yTicks.labels(axes.coordinates.port_area.x1, -1);

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