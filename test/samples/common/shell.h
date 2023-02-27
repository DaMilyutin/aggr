#pragma once

#include <agge/rendering/platform.h>

struct services;

#include <system_events.h>

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


    system_input::EventAggregator events;

	virtual ~application();

	virtual void draw(agge::platform_bitmap &surface, timings &timings_) = 0;
	virtual void resize(int width, int height);

    virtual bool consume_events() { return true; }
};



extern application *agge_create_application(services &/*s*/);
