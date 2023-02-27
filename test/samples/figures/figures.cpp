#include <agge/rendering/clipper.h>
#include <agge/rendering/filling_rules.h>
#include <agge/rendering/renderer.h>
#include <agge/rendering/rasterizer.h>

#include <agge/primitives/curves.h>
#include <agge/primitives/dash.h>
#include <agge/primitives/figures.h>
#include <agge/primitives/path.h>
#include <agge/primitives/stroke.h>
#include <agge/primitives/stroke_features.h>

#include <samples/common/shell.h>

using namespace agge;

namespace
{
	template <typename T>
	rect<T> mkrect(T x1, T y1, T x2, T y2)
	{
		rect<T> r = { x1, y1, x2, y2 };
		return r;
	}

	class Figures : public application
	{
	public:
		Figures()
		{
			_line_style.width(4.0f);
			_line_style.set_cap(caps::butt());
			_line_style.set_join(joins::bevel());
            _dash.dash_start(0.5f);
			_dash.add_dash(1.0f, 1.0f);
		}

	private:
		virtual void draw(platform_bitmap &surface, timings &/*timings*/)
		{
			ras.reset();
            auto wid = float(surface.width());
            auto hei = float(surface.height());
            auto scale = std::min(wid/800.0f, hei/670.0f);

			_line_style.width(scale*15.0f);
			add_path(ras, agge::Interval(0.02f)/qbezier_interp({scale*10.0f, scale*150.0f}, {scale*440.0f, scale*300.0f}, {scale*200.0f, scale*150.0f})/_line_style);
			add_path(ras, arc(scale*105.0f, scale*140.0f, scale*95.0f, 1.05f * pi, 1.95f * pi)/_line_style);
			_line_style.width(scale*1.0f);

			add_path(ras, rounded_rectangle(scale*10.0f, scale*260.0f, scale*111.0f, scale*311.0f, scale*7.0f));

            add_path(ras, rounded_rectangle(scale*10.5f, scale*200.5f, scale*110.5f, scale*250.5f, scale*7.0f)/_dash/_line_style);

			ras.sort();

			fill(surface, mkrect<int>(0, 0, surface.width(), surface.height()),
				platform_blender_solid_color(color::make(0, 50, 100)));
			ren(surface, zero(), 0 /*no windowing*/, ras /*mask*/,
				platform_blender_solid_color(color::make(255, 255, 255)), winding<>());
		}

	private:
		rasterizer< clipper<int> > ras;
		renderer ren;
		stroke _line_style;
		dash _dash;
	};
}

application *agge_create_application(services &/*s*/)
{
	return new Figures;
}
