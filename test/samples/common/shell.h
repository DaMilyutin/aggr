#pragma once

#include <agge/rendering/platform.h>

struct services;

struct application : agge::noncopyable
{
	struct timings
	{
		double clearing;
		double stroking;
		double rasterization;
		double rendition;
		double blitting;
	};

	virtual ~application();

	virtual void draw(agge::platform_bitmap &surface, timings &timings_) = 0;
	virtual void resize(int width, int height);
};



extern application *agge_create_application(services &/*s*/);
