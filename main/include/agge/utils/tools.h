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

    class noncopyable
    {
    public:
        noncopyable() throw() { }

    private:
        noncopyable(const noncopyable& other) = delete;
        const noncopyable& operator =(const noncopyable& rhs) = delete;
    };
}
