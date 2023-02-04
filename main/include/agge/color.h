#pragma once

#include "types.h"

namespace agge
{
	struct color
	{
		uint8_t r, g, b, a;
        static constexpr uint8_t u255 = uint8_t(255);

        color inverted() const { return {uint8_t(255-r), uint8_t(255-g), uint8_t(255-b), a}; }

		static color make(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 0xFF);
	};
}
