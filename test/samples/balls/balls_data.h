#pragma once

#include <agge/color.h>
#include <vector>
#include <cmath>

struct ball
{
	ball()
		: radius(1.0f), color(agge::color::make(128, 128, 128, 10)), vx(1.0f), vy(1.0f), x(1.0f), y(1.0f)
	{	}

	ball(agge::real_t radius_, agge::color color_, agge::real_t vx_, agge::real_t vy_, agge::real_t x_, agge::real_t y_)
		: radius(radius_), color(color_), vx(vx_), vy(vy_), x(x_), y(y_)
	{	}

	agge::real_t radius;
	agge::color color;
	agge::real_t vx, vy;
	agge::real_t x, y;
};

inline void move_and_bounce(ball &ball_, agge::real_t dt, agge::real_t w, agge::real_t h)
{
	const agge::real_t radius = ball_.radius;
	const agge::real_t l = radius, r = w - radius, t = radius, b = h - radius;
	agge::real_t &x = ball_.x, &y = ball_.y;
	agge::real_t &vx = ball_.vx, &vy = ball_.vy;

	x += dt * vx, y += dt * vy;

	if (x < l)
		x += l - x, vx = -vx;
	else if (x > r)
		x -= x - r, vx = -vx;

	if (y < t)
		y += t - y, vy = -vy;
	else if (y > b)
		y -= y - b, vy = -vy;
}

inline float sq(float x) { return x * x; }

inline void move_and_bounce_with_grav(ball& ball_, agge::real_t dt, agge::real_t w, agge::real_t h)
{
	const agge::real_t radius = ball_.radius;
	const agge::real_t l = radius, r = w - radius, t = radius, b = h - radius;
	agge::real_t& x = ball_.x, & y = ball_.y;
	agge::real_t& vx = ball_.vx, & vy = ball_.vy;


	x += dt * vx, y += dt * vy;

	if (x < l)
		x += l - x, vx = -vx;
	else if (x > r)
		x -= x - r, vx = -vx;

	if (y < t)
		y += t - y, vy = -vy;
	else if (y > b)
		y -= y - b, vy = -vy;


	const float dx = x - w / 2, dy = y - h / 2;
	const float R = std::sqrt((w * w + h * h) / 100);
	const float d = std::sqrt(sq(dx) + sq(dy));

	//if (d - R < radius)
	//{
	//	auto const vR = (dx * vx + dy * vy)/(d*d);
	//	if(vR < 0)
	//	{
	//		//auto const np = std::sqrt(vx * vx + vy * vy);
	//		vx += -2*vR * dx;
	//		vy += -2*vR * dy;
	//		//auto const t = np/std::sqrt(vx * vx + vy * vy);
	//		//vx *= t;
	//		//vy *= t;
	//	}
	//}

	if (d - R < 0)
	{
		auto vr = (vx * dy - vy * dx)/d;
		auto F = vr*0.1;
		vx += -vy * F * dt;
		vy +=  vx * F * dt;
	}

	if (std::hypot(vx, vy) > 2.)
	{
		vx *= (1.f - 0.01*dt);
		vy *= (1.f - 0.01*dt);
	}
}

extern const std::vector<ball> c_balls;
