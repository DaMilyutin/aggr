#pragma once

#include <agge/primitives/polyline.h>
#include <agge/types.h>
#include <math.h>
#include <vector>

namespace common
{
	inline float fabs(float value)
	{	return ::fabsf(value);	}

	inline double fabs(double value)
	{	return ::fabs(value);	}

	inline float acos(float value)
	{	return ::acosf(value);	}

	inline double acos(double value)
	{	return ::acos(value);	}

	typedef agge::polyline AggPath;

	class line_adaptor
	{
	public:
		line_adaptor(agge::real_t x1, agge::real_t y1, agge::real_t x2, agge::real_t y2)
			: _x1(x1), _y1(y1), _x2(x2), _y2(y2), _pos(0)
		{	}

		void rewind(unsigned)
		{
			_pos = 0;
		}

		unsigned vertex(agge::real_t* x, agge::real_t* y)
		{
			switch (_pos)
			{
			case 0:
				*x = _x1, *y = _y1;
				++_pos;
				return agge::path_command_move_to;

			case 1:
				*x = _x2, *y = _y2;
				++_pos;
				return agge::path_command_line_to;

			default:
				return agge::path_command_stop;
			}
		}

	private:
		agge::real_t _x1, _y1, _x2, _y2;
		int _pos;
	};


	template <typename RealT, typename PathT>
	inline void flatten(agge::polyline& destination, PathT source)
	{
		unsigned cmd;
		RealT x, y;

		source.rewind(0);
		while (cmd = source.vertex(&x, &y), cmd != agge::path_command_stop)
			destination.push_back(agge::polyline::Item{x, y, cmd});
	}

	template <typename TargetT>
	inline void spiral(TargetT &target, agge::real_t x, agge::real_t y, agge::real_t r1, agge::real_t r2,
		agge::real_t step, agge::real_t start_angle)
	{
		const agge::real_t k = 4.0f;

		bool start = true;

		target.start();
		for (agge::real_t angle = start_angle, dr = k * step / 45.0f, da = k / 180.0f * agge::pi;
			r1 < r2; r1 += dr, angle += da, start = false)
		{
			const agge::real_t px = x + agge::cos(angle) * r1, py = y + agge::sin(angle) * r1;

			if (start)
                target.move_to(px, py);
			else
                target.line_to(px, py);
		}
		target.stop();
	}
}
