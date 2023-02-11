#pragma once

#include <plotting/types/rasteriser.h>
#include <plotting/primitives/Canvas.h>

#include <complex>
#define _USE_MATH_DEFINES
#include <math.h>

namespace plotting
{
    struct MarkerPolygonProperties
    {
        MarkerPolygonProperties() = default;
        MarkerPolygonProperties(MarkerPolygonProperties&&) = default;
        MarkerPolygonProperties(MarkerPolygonProperties const&) = default;
        MarkerPolygonProperties(int N, agge::real_t radius): N(N), radius(radius) {}
        MarkerPolygonProperties& shape(int iN, int iM = 1) { N = iN; M = iM; return *this; }
        MarkerPolygonProperties& size(agge::real_t r, agge::real_t o = 0.0f) { radius = r; offset = agge::real_t(o*M_PI/180.0); return *this; }

        agge::real_t radius = 5.0f;
        agge::real_t offset = 0.0f;
        int          N = 3;
        int          M = 1;
       // agge::color  fill = agge::color::make(255, 0, 0);
    };

    struct MarkerPolygon: MarkerPolygonProperties
    {
        MarkerPolygon() = default;
        MarkerPolygon(MarkerPolygon const&) = default;
        MarkerPolygon(MarkerPolygon&&) = default;

        MarkerPolygon(MarkerPolygonProperties const& p): MarkerPolygonProperties(p) {}
        MarkerPolygon(MarkerPolygonProperties&& p): MarkerPolygonProperties(std::move(p)) {}

        using MarkerPolygonProperties::MarkerPolygonProperties;

        MarkerPolygonProperties& shape(int iN, int iM = 1) { N = iN; M = iM; return *this; }
        MarkerPolygonProperties& size(agge::real_t r, agge::real_t o = 0.0f) { radius = r; offset = o; return *this; }
        MarkerPolygon& position(agge::point_r const& p) { center = p; return *this; }

        agge::point_r center {0.0f, 0.0f};
    };


    template<typename S, typename Rd, typename Rs>
    Canvas<S, Rd, Rs>& operator<<(Canvas<S, Rd, Rs>& c, MarkerPolygon const& m)
    {
        std::complex<agge::real_t> const rot = std::polar(1.0f, float(m.M*2.*M_PI/m.N));
        std::complex<agge::real_t> point = std::polar(m.radius, m.offset - float(M_PI_2));
        c.ras.move_to(m.center.x + point.real(), m.center.y + point.imag()); point *= rot;
        for(int i = 1; i < m.N; ++i, point *= rot)
            c.ras.line_to(m.center.x + point.real(), m.center.y + point.imag());
        c.ras.close_polygon();
        c.ras.sort();
        return c;// << m.fill << plotting::reset;
    }
}