#pragma once
#include "CoordinateSystem.h"
#include <agge/color.h>
#include <agge/primitives/figures.h>
#include <agge/primitives/stroke.h>
#include <agge/primitives/stroke_features.h>

#include <agge/rendering/platform.h>
#include <agge/rendering/renderer.h>


#include <plotting/Canvas.h>
#include <plotting/LinesGen.h>


namespace plotting
{
    struct Axes
    {
        port_area_t      position;
        CoordinateSystem coordinates;

        agge::color outer { 40, 40, 40, 128};
        agge::color inner { 20, 20, 20, 255};

        struct TickProperties
        {
            float length = 7;
            float width = 1;
            agge::color color = {255, 255, 255, 128};
        };

        struct Separator
        {
            agge::color color = {255, 255, 255, 128};
            float width = 2.;
        };

        struct AxisProperties
        {
            TickProperties tick[3] = {{10.0f,1.5f}, {7.0f, 1.5f}, {4.0f, 1.5f}}; // major middle, minor
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
        //struct Selector_All
        //{
        //    bool is() const { return true; }
        //    void next() {}
        //};

        //struct Selector_Skipper
        //{
        //    int index  = 0;
        //    int period = 5;
        //    bool is() const { return index%period != 0; }
        //    void next() { ++index; }
        //};

        //struct LineVert
        //{
        //    agge::real_t const y1;
        //    agge::real_t const y2;

        //    agge::line make(agge::real_t x) const { return agge::line(x, y1, x, y2); }
        //};

        //struct LineHorz
        //{
        //    agge::real_t const x1;
        //    agge::real_t const x2;

        //    agge::line make(agge::real_t y) const { return agge::line(x1, y, x2, y); }
        //};

        //template<typename LineMaker, typename Selector = Selector_All>
        //struct Range
        //{
        //    Selector  mutable   sel;
        //    LineMaker           line;

        //    agge::real_t const  t1;
        //    agge::real_t const  t2;

        //    agge::real_t const  start;
        //    agge::real_t const  step;

        //    agge::color  const& color;
        //    agge::stroke&       line_style;
        //};

        //template<typename S, typename Rn, typename Rs, typename Line, typename Sel>
        //plotting::Canvas<S, Rn, Rs>& operator<<(plotting::Canvas<S, Rn, Rs>& c,
        //                                        Range<Line, Sel> const& rng)
        //{
        //    c.ras.reset();
        //    agge::real_t t = rng.start;
        //    while(t < rng.t1)
        //        t += rng.step, rng.sel.next();
        //    for(; t<=rng.t2; t += rng.step, rng.sel.next())
        //        if(rng.sel.is())
        //            agge::add_path(c.ras, rng.line.make(t)/rng.line_style);
        //    c.ras.sort();
        //    c.ren(c.surface, agge::zero(), 0 /*no windowing*/, c.ras /*mask*/,
        //          agge::platform_blender_solid_color(rng.color), agge::winding<>());
        //    return c;
        //}

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
            AxisXTicksMaker(Axes const& axes, Axes::AxisProperties const& prop, agge::stroke& line_style)
                : axes(axes)
                , prop(prop)
                , line_style(line_style)
            {
                double const step_repr = quantize_step((axes.coordinates.repr_area.x2 - axes.coordinates.repr_area.x1)/prop.tickSteps);
                double const X_repr = step_repr*ceil(axes.coordinates.repr_area.x1/step_repr);
                start = agge::real_t(axes.coordinates.scale.x*(X_repr - axes.coordinates.repr_area.x1) + axes.coordinates.port_area.x1);
                step  = agge::real_t(step_repr*axes.coordinates.scale.x);
            }

            MajorLines major(agge::real_t Y, int dir) const
            {
                line_style.width(prop.tick[0].width);
                auto const& pa = axes.coordinates.port_area;

                ParallelLinesGenerator<Selector_Any> gen;
                gen.initial.start = {start, Y};
                gen.initial.end = {start, Y+dir*prop.tick[0].length};
                gen.direction = {step, 0.0f};
                gen.number = (int)ceil((pa.x2-start)/step);
                return {gen, StylishLineMaker{line_style}, prop.tick[0].color};
            }

            MajorLines medium(agge::real_t Y, int dir) const
            {
                line_style.width(prop.tick[1].width);
                auto const& pa = axes.coordinates.port_area;
                agge::real_t from = start-step*0.5f;
                if(from < pa.x1)
                    from += step;
                ParallelLinesGenerator<Selector_Any> gen;
                gen.initial.start = {from, Y};
                gen.initial.end = {from, Y+dir*prop.tick[1].length};
                gen.direction = {step, 0.0f};
                gen.number = (int)ceil((pa.x2-from)/step);
                return {gen, StylishLineMaker{line_style}, prop.tick[1].color};
            }

            MinorLines minor(agge::real_t Y, int dir) const
            {
                line_style.width(prop.tick[2].width);
                auto const& pa = axes.coordinates.port_area;
                agge::real_t const inc = step*0.1f;
                agge::real_t from = start - inc*9.0f;
                ParallelLinesGenerator<Selector_SkipOverPeriod> gen;
                gen.select.offset = 1;
                while(from <= pa.x1)
                    from += inc, ++gen.select.offset;
                gen.select.offset = gen.select.offset % gen.select.period;
                gen.initial.start = {from, Y};
                gen.initial.end = {from, Y+dir*prop.tick[2].length};
                gen.direction = {inc, 0.0f};
                gen.number = (int)ceil((pa.x2-from)/inc);
                return {gen, StylishLineMaker{line_style}, prop.tick[2].color};
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
        return c;
    }

    template<typename S, typename Rn, typename Rs>
    Canvas<S, Rn, Rs>& operator<<(Canvas<S, Rn, Rs>& c, Axes const& axes)
    {
        agge::stroke line_style;
        line_style.set_cap(agge::caps::butt());
        line_style.set_join(agge::joins::bevel());
        c << inner::AxesArea{axes, line_style};

        {
            inner::AxisXTicksMaker xTicks{axes, axes.properties.x1, line_style};
            c << xTicks.major(axes.coordinates.port_area.y1,-1);
            c << xTicks.medium(axes.coordinates.port_area.y1, -1);
            c << xTicks.minor(axes.coordinates.port_area.y1, -1);
        }
        return c;
    }
}