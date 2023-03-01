#pragma once

#include <agge/types.h>

namespace agge
{
	struct zero
	{
		template <typename T>
		operator T() const
		{
			T v = {};
			return v;
		}
	};
}
