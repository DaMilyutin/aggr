#pragma once

#include "range_adapter.h"
#include "shared_ptr.h"
#include "types.h"

#include <agge/utils/pod_vector.h>
#include <agge/utils/tools.h>
#include <vector>

namespace agge
{
	class font;
	struct glyph_run;
	struct positioned_glyph;
	struct text_line;

	typedef std::vector<glyph_run> glyph_runs_container_t;
	typedef pod_vector<positioned_glyph> positioned_glyphs_container_t;
	typedef std::vector<text_line> text_lines_container_t;

	struct positioned_glyph
	{
		glyph_index_t index;
		Vector_r d;
	};

	struct glyph_run : range_adapter<const positioned_glyphs_container_t>
	{
		glyph_run(const positioned_glyphs_container_t &container);

		shared_ptr<font> font_;
		Vector_r offset;
	};

	struct text_line : range_adapter<const glyph_runs_container_t>
	{
		text_line(const glyph_runs_container_t &container);

		Vector_r offset;
		real_t extent;
		real_t descent;
	};



	inline glyph_run::glyph_run(const positioned_glyphs_container_t &container)
		: range_adapter<const positioned_glyphs_container_t>(container), offset(zero())
	{	}


	inline text_line::text_line(const glyph_runs_container_t &container)
		: range_adapter<const glyph_runs_container_t>(container), offset(zero()), extent(real_t())
	{	}
}
