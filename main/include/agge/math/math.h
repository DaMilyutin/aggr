#pragma once

#include <agge/types.h>

namespace agge
{
	extern const real_t c_qarc_bezier_k;
	extern const real_t distance_epsilon;
	extern const real_t pi;

	template <typename T>
	struct limits
	{
		static T resolution();
	};

	real_t sqrt(real_t x);
	real_t sin(real_t a);
	real_t cos(real_t a);
	real_t acos(real_t v);

    double sqrt(double x);

	inline int iround(real_t v)
	{	return static_cast<int>(v > real_t() ? v + static_cast<real_t>(0.5) : v - static_cast<real_t>(0.5));	}

	int muldiv(int a, int b, int c);

	inline real_t muldiv(real_t a, real_t b, real_t c)
	{	return a * b / c;	}

	real_t optimal_circle_stepping(real_t radius, real_t scale = 1.0f);




}
