#pragma once
#include <agge/rendering/blenders_generic.h>
#include <agge/rendering/blenders.h>
#include <agge/utils/tools.h>
#include <agge/primitives/pipeline.h>
#include <agge/rendering/filling_rules.h>

#include <plotting/piping/rules.h>

namespace plotting
{
    template<typename SurfaceT, typename RendererT, typename RasterizerT>
    struct Canvas: public piping::Sink<Canvas<SurfaceT, RendererT, RasterizerT>>
    {
    public:
        SurfaceT&    surface;
        RendererT&   ren;
        RasterizerT& ras;

        template<typename PixelT, typename OrderT>
        Canvas& operator<<(agge::blender_solid_color_rgb<PixelT, OrderT>& bl)
        {
            ras.sort();
            ren(surface, agge::zero(), 0 /*no windowing*/, ras /*mask*/,
                bl, agge::winding<>());
            ras.reset();
            return *this;
        }

        template<typename PixelT, typename OrderT>
        Canvas& operator<<(agge::blender_solid_color_rgb<PixelT, OrderT>&& bl)
        {
            ras.sort();
            ren(surface, agge::zero(), 0 /*no windowing*/, ras /*mask*/,
                bl, agge::winding<>());
            ras.reset();
            return *this;
        }

        template<typename PixelT, typename OrderT>
        Canvas& operator<<(agge::blender_solid_color<PixelT, OrderT>& bl)
        {
            ras.sort();
            ren(surface, agge::zero(), 0 /*no windowing*/, ras /*mask*/,
                bl, agge::winding<>());
            ras.reset();
            return *this;
        }

        template<typename PixelT, typename OrderT>
        Canvas& operator<<(agge::blender_solid_color<PixelT, OrderT>&& bl)
        {
            ras.sort();
            ren(surface, agge::zero(), 0 /*no windowing*/, ras /*mask*/,
                bl, agge::winding<>());
            ras.reset();
            return *this;
        }

        Canvas& operator<<(agge::color const& col)
        {
            ras.sort();
            ren(surface, agge::zero(), 0 /*no windowing*/, ras /*mask*/,
                agge::platform_blender_solid_color(col), agge::winding<>());
            ras.reset();
            return *this;
        }

        using canvas_fcn = Canvas&(*)(Canvas&);

        template<typename E>
        Canvas& operator<<(agge::pipeline::terminal<E>& e)
        {
            add_path(ras, e._get_());
            return *this;
        }

        template<typename E>
        Canvas& operator<<(agge::pipeline::terminal<E>&& e)
        {
            add_path(ras, e._get_());
            return *this;
        }

        template<typename E>
        Canvas& operator<<(piping::Yield<E> const& e)
        {
            meld_tag<piping::terminal>(e, *this);
            return *this;
        }

        template<typename E>
        bool consume(E const& e)
        {
            *this << e;
            return true;
        }

        template<typename E>
        bool consume(E&& e)
        {
            *this << e;
            return true;
        }

        template<typename E>
        Canvas& operator<<(piping::Yield<E>&& e)
        {
            meld_tag<piping::terminal>(FWD(e), *this);
            return *this;
        }

        Canvas& operator<<(canvas_fcn f)
        {
            return f(*this);
        }
    };

    template<typename S, typename Rd, typename Rs>
    Canvas<S, Rd, Rs>& reset(Canvas<S, Rd, Rs>& c)
    {
        c.ras.reset();
        return c;
    }


    template<typename S, typename Rd, typename Rs>
    inline Canvas<S, Rd, Rs> make_canvas(S& s, Rd& ren, Rs& ras)
    {
        return Canvas<S, Rd, Rs>{{}, s, ren, ras};
    }
}