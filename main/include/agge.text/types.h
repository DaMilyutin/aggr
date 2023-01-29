#pragma once

#include <agge/types.h>
#include <string>

namespace agge
{
	typedef unsigned int glyph_index_t;
	typedef unsigned int codepoint_t;

	enum text_alignment {
		align_near,	// vertical: top, horizontal: LTR -> left, RTL -> right
		align_far,	// vertical: bottom, horizontal: LTR -> right, RTL -> left
		align_center,
	};

	struct full_alignment {
		static full_alignment create(text_alignment halign_, text_alignment valign_);

		text_alignment halign : 3, valign : 3;
	};

	enum font_weight {
		extra_light,
		light,
		book,
		regular,
		medium,
		semi_bold,
		bold,
		black,
		extra_black,
	};

	enum font_hinting {
		hint_none,
		hint_vertical,
		hint_strong,
	};

	struct font_metrics
	{
		real_t ascent;
		real_t descent;
		real_t leading;
	};

	struct font_descriptor
	{
		static font_descriptor create(const std::string &family, int height, font_weight weight = regular,
			bool italic = false, font_hinting hinting_ = hint_none);

		std::string family;
		int height : 20;
		font_weight weight : 4;
		unsigned italic : 1;
		font_hinting hinting : 2;
	};



	bool operator <(const font_descriptor &lhs, const font_descriptor &rhs);
	bool operator ==(const font_descriptor &lhs, const font_descriptor &rhs);
}
