#pragma once

namespace agge
{
	template <typename T>
	inline void update_min(T &value, T candidate)
	{
		if (candidate < value)
			value = candidate;
	}

	template <typename T>
	inline void update_max(T &value, T candidate)
	{
		if (candidate > value)
			value = candidate;
	}

	template <typename T>
	inline T min(T lhs, T rhs)
	{	return lhs < rhs ? lhs : rhs;	}

	template <typename T>
	inline T max(T lhs, T rhs)
	{	return lhs > rhs ? lhs : rhs;	}
}
