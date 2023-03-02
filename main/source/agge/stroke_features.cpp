#include <agge/primitives/stroke_features.h>

#include <agge/primitives/curves.h>
#include <agge/math/math.h>
#include <agge/utils/tools.h>

namespace agge
{
	namespace caps
	{
		void butt::calc(points &output, real_t hw, const Point_r &v0, real_t d, const Point_r &v1) const
		{
			d = hw / d;

			const real_t dx = (v1.x - v0.x) * d;
			const real_t dy = (v1.y - v0.y) * d;

			output.push_back(create_point(v0.x - dy, v0.y + dx));
			output.push_back(create_point(v0.x + dy, v0.y - dx));
		}


		triangle::triangle(real_t tip_extension)
			: _tip_extension(tip_extension)
		{	}

		void triangle::calc(points &output, real_t hw, const Point_r &v0, real_t d, const Point_r &v1) const
		{
			d = hw / d;

			const real_t dx = (v1.x - v0.x) * d;
			const real_t dy = (v1.y - v0.y) * d;

			output.push_back(create_point(v0.x - dy, v0.y + dx));
			output.push_back(create_point(v0.x - dx * _tip_extension, v0.y - dy * _tip_extension));
			output.push_back(create_point(v0.x + dy, v0.y - dx));
		}


		void round::calc(points &output, real_t hw, const Point_r &v0, real_t d, const Point_r &v1) const
		{
			d = hw / d;

			const real_t step = 4.0f * optimal_circle_stepping(hw);
			const real_t dx = (v1.x - v0.x) * d, d2x = c_qarc_bezier_k * dx;
			const real_t dy = (v1.y - v0.y) * d, d2y = c_qarc_bezier_k * dy;

            cbezier_interp interp;
            interp._b = {v0.x - dy, v0.y + dx};
            interp._c1 = {interp._b.x - d2x, interp._b.y - d2y};
            interp._e = {v0.x - dx, v0.y - dy};
            interp._c2 = {interp._e.x - d2y, interp._e.y + d2x};


            output.push_back(interp._b);
			for (real_t t = step; t < 1.0f; t += step)
			{
				Point_r p = interp.at(t);
				output.push_back(p);
			}
			output.push_back(interp._e);

            interp._b = interp._e;
            interp._c1 = {interp._b.x + d2y, interp._b.y - d2x};
            interp._e = {v0.x + dy, v0.y - dx};
            interp._c2 = {interp._e.x - d2x, interp._e.y - d2y};

			for (real_t t = step; t < 1.0f; t += step)
			{
				Point_r p = interp.at(t);
				output.push_back(p);
			}
			output.push_back(interp._e);
		}
	}

	namespace joins
	{
		void bevel::calc(points &output, real_t hw, const Point_r &v0, real_t d01, const Point_r &v1, real_t d12, const Point_r &v2) const
		{
			d01 = hw / d01;
			d12 = hw / d12;

			const real_t dx1 = d01 * (v1.x - v0.x);
			const real_t dy1 = d01 * (v1.y - v0.y);
			const real_t dx2 = d12 * (v2.x - v1.x);
			const real_t dy2 = d12 * (v2.y - v1.y);

			output.push_back(create_point(v1.x + dy1, v1.y - dx1));
			output.push_back(create_point(v1.x + dy2, v1.y - dx2));
		}
	}
}
