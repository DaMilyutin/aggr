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
            Formatter   format    = [](double x) { return std::to_string(x); };
            std::string font      = "Courier";
            int         font_size = 10;
        };


        struct TickProperties
        {
            agge::real_t length = 7.0f;
            agge::real_t width  = 1.0f;
            agge::color color = {255, 255, 255, 128};
        };

        struct Separator
        {
            agge::color color = {255, 255, 255, 128};
            float width = 2.;
        };

        struct AxisProperties
        {
            TickProperties tick[3] = {{7.0f,1.5f}, {5.0f, 1.0f}, {2.0f,1.0f}}; // major middle, minor
            Separator      sep;
            int            tickSteps = 5;
        };

        agge::rect<AxisProperties> properties;

        void alignTickStepsX1(double dx)
        {
            auto const& pa = coordinates.port_area;
            properties.x1.tickSteps = int(ceil(fabs(pa.x2-pa.x1)/dx));
        }

        void alignTickStepsX2(double dx)
        {
            auto const& pa = coordinates.port_area;
            properties.x2.tickSteps = int(ceil(fabs(pa.x2-pa.x1)/dx));
        }

        void alignTickStepsY1(double dy)
        {
            auto const& pa = coordinates.port_area;
            properties.y1.tickSteps = int(ceil(fabs(pa.y2-pa.y1)/dy));
        }

        void alignTickStepsY2(double dy)
        {
            auto const& pa = coordinates.port_area;
            properties.y1.tickSteps = int(ceil(fabs(pa.y2-pa.y1)/dy));
        }

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

        struct AxesArea
        {
            Axes const&   axes;
            agge::stroke& line_style;
        };

        using MajorLines = LinesGenerator<ParallelLinesGenerator<Selector_Any>, StylishLineMaker>;
        using MinorLines = LinesGenerator<ParallelLinesGenerator<Selector_SkipOverPeriod>, StylishLineMaker>;

        struct AxisXTicksMaker
        {
            static float grow_step(float step)
            {
                constexpr float min_resolution = 100.0f;
                if(step < min_resolution)
                    return ceil(min_resolution/step)*step;
                return step;
            }

            AxisXTicksMaker(Axes const& axes, Axes::AxisProperties const& prop, agge::stroke& line_style)
                : axes(axes)
                , prop(prop)
                , line_style(line_style)
            {
                double const step_repr = quantize_step((axes.coordinates.repr_area.x2 - axes.coordinates.repr_area.x1)/prop.tickSteps);
                step = grow_step(agge::real_t(step_repr*axes.coordinates.scale.x));
                float mid = 0.5f*(axes.coordinates.port_area.x2 + axes.coordinates.port_area.x1);
                float mid_q = round(mid/step)*step;
                start = mid_q - floor((mid_q - axes.coordinates.port_area.x1)/step)*step;
            }

            MajorLines major(agge::real_t Y, int dir) const
            {
                line_style.width(prop.tick[0].width);
                auto const& pa = axes.coordinates.port_area;
                agge::real_t from = start;
                if(from < pa.y1 + 0.1f)
                    from += step;
                ParallelLinesGenerator<Selector_Any> gen;
                gen.initial.start = {from, Y};
                gen.initial.end = {from, Y+dir*prop.tick[0].length};
                gen.direction = {step, 0.0f};
                gen.number = (int)ceil((pa.x2 - 0.1 - from)/step);
                return {gen, StylishLineMaker{line_style}, prop.tick[0].color};
            }

            MajorLines medium(agge::real_t Y, int dir) const
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

            MinorLines minor(agge::real_t Y, int dir) const
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
            static float grow_step(float step)
            {
                constexpr float min_resolution = 50.0f;
                if(step < min_resolution)
                    return ceil(min_resolution/step)*step;
                return step;
            }

            AxisYTicksMaker(Axes const& axes, Axes::AxisProperties const& prop, agge::stroke& line_style)
                : axes(axes)
                , prop(prop)
                , line_style(line_style)
            {
                double const step_repr = quantize_step((axes.coordinates.repr_area.y2 - axes.coordinates.repr_area.y1)/prop.tickSteps);
                step = grow_step(agge::real_t(step_repr*axes.coordinates.scale.y));
                float mid = 0.5f*(axes.coordinates.port_area.y2 + axes.coordinates.port_area.y1);
                float mid_q = round(mid/step)*step;
                start = mid_q - floor((mid_q - axes.coordinates.port_area.y1)/step)*step;
            }

            MajorLines major(agge::real_t X, int dir) const
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

            MajorLines medium(agge::real_t X, int dir) const
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

            MinorLines minor(agge::real_t X, int dir) const
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
        }
        {
            inner::AxisXTicksMaker xTicks{axes, axes.properties.x2, line_style};
            c << xTicks.major(axes.coordinates.port_area.y2, 1);
            c << xTicks.medium(axes.coordinates.port_area.y2, 1);
            c << xTicks.minor(axes.coordinates.port_area.y2, 1);
        }
        {
            inner::AxisYTicksMaker yTicks{axes, axes.properties.y1, line_style};
            c << yTicks.major(axes.coordinates.port_area.x1, -1);
            c << yTicks.medium(axes.coordinates.port_area.x1, -1);
            c << yTicks.minor(axes.coordinates.port_area.x1, -1);
        }
        {
            inner::AxisYTicksMaker yTicks{axes, axes.properties.y2, line_style};
            c << yTicks.major(axes.coordinates.port_area.x2, 1);
            c << yTicks.medium(axes.coordinates.port_area.x2, 1);
            c << yTicks.minor(axes.coordinates.port_area.x2, 1);
        }
        return c;
    }
}