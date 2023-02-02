#include <agge/rendering/clipper.h>
#include <agge/rendering/renderer.h>
#include <agge/rendering/rasterizer.h>
#include <agge/rendering/filling_rules.h>

#include <agge/primitives/curves.h>
#include <agge/primitives/dash.h>
#include <agge/primitives/figures.h>
#include <agge/primitives/path.h>
#include <agge/primitives/stroke.h>
#include <agge/primitives/stroke_features.h>

#include <samples/common/shell.h>

#include <algorithm>

using namespace agge;

namespace
{
	template <typename T>
	rect<T> mkrect(T x1, T y1, T x2, T y2)
	{
		rect<T> r = { x1, y1, x2, y2 };
		return r;
	}

	class Lines : public application
	{
	private:
		void knot(real_t x, real_t y, real_t scale = 1.0f)
		{
            float const dx = 30.0f*scale;
            float const dy = 0.0f*scale;
            x += dx;
            y += dy;
			add_path(ras,
                    assist(cbezier(x - 25.0f*scale, y - 15.0f*scale, x + 80.0f*scale, y + 200.0f*scale,
				                              x - 80.0f*scale, y + 200.0f*scale, x + 25.0f, y - 15.0f*scale, 0.01f*scale), line_style));
		}

		virtual void draw(platform_bitmap &surface, timings &/*timings*/)
		{
			ras.reset();
            auto wid = float(surface.width());
            auto hei = float(surface.height());
            auto scale = std::min(wid/400.0f, hei/370.0f);
			line_style.width(15.0f*scale);
			line_style.set_cap(caps::triangle(3.0f*scale));
			line_style.set_join(joins::bevel());
			knot(40.0f*scale, 100.0f*scale, scale);

			line_style.width(5.0f*scale);
			line_style.set_cap(caps::butt());
			line_style.set_join(joins::bevel());
			knot(110.0f*scale, 100.0f*scale, scale);

			line_style.width(10.0f*scale);
			line_style.set_cap(caps::triangle(-3.0f));
			line_style.set_join(joins::bevel());
			knot(180.0f*scale, 100.0f*scale, scale);

			line_style.width(10.0f*scale);
			line_style.set_cap(caps::round());
			line_style.set_join(joins::bevel());
			knot(250.0f*scale, 100.0f*scale, scale);

			line_style.width(15.0f*scale);
			line_style.set_cap(caps::round());
			line_style.set_join(joins::bevel());
			knot(320.0f*scale, 100.0f*scale, scale);

			line_style.width(1.0f*scale);
			line_style.set_cap(caps::butt());
			dash_style.remove_all_dashes();
			dash_style.add_dash(1.0f*scale, 1.0f*scale);
			dash_style.dash_start(0.5f*scale);
			add_path(ras,
            assist(assist(line(19.5f*scale, 300.5f*scale, 319.5f*scale, 300.5f*scale), dash_style), line_style));

			ras.sort();

			fill(surface, mkrect<int>(0, 0, surface.width(), surface.height()),
				platform_blender_solid_color(color::make(0, 50, 100)));
			ren(surface, zero(), 0 /*no windowing*/, ras /*mask*/,
				platform_blender_solid_color(color::make(255, 255, 255)), winding<>());
		}

	private:
		rasterizer< clipper<int> > ras;
		renderer ren;
		stroke   line_style;
		dash     dash_style;
	};
}

application *agge_create_application(services &/*s*/)
{
	return new Lines;
}
