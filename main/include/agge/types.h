#pragma once

#include <agge/types/Point.h>
#include <agge/types/Rect.h>

namespace agge
{
	class noncopyable
	{
	public:
		noncopyable() throw() { }

	private:
		noncopyable(const noncopyable &other) = delete;
		const noncopyable &operator =(const noncopyable &rhs) = delete;
	};
}
