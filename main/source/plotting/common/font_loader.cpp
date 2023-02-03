#include <plotting/common/font_loader.h>
#include <plotting/common/truetype.h>

#include <plotting/common/serialization.h>

#include <stdlib.h>
#include <strmd/deserializer.h>
#include <strmd/container_ex.h>
//#include <tests/common/scoped_ptr.h>

using namespace agge;
using namespace std;
//using namespace strmd;

namespace
{
	string to_string(int value)
	{
		char buffer[20] = { };

		sprintf(buffer, "%d", value);
		return buffer;
	}

	string format_font_name(const string &family, int height, font_weight weight, bool italic, font_hinting grid_fit)
	{
		return family + "-" + to_string(height) + (weight >= bold ? "b" : "") + (italic ? "i" : "")
			+ (grid_fit == hint_strong ? "h" : grid_fit == hint_vertical ? "v" : "") + ".fnt";
	}
}

font_loader::font_loader(services &s)
	: _services(s)
{
}

font::accessor_ptr font_loader::load(const font_descriptor &descriptor)
{
	std::unique_ptr<stream> r(_services.open_file(format_font_name(descriptor.family, descriptor.height,
		descriptor.weight, descriptor.italic, descriptor.hinting).c_str()));
	strmd::deserializer<stream, strmd::varint> dser(*r);
	shared_ptr<truetype::font> font(new truetype::font);

	dser(*font);
	return truetype::create_accessor(font, descriptor);
}
