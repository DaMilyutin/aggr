#pragma once
#include <plotting/primitives/EntitiesGenerator.h>
#include <plotting/primitives/Text.h>

#include <functional>
#include <plotting/generators/iota.h>
#include <plotting/generators/zip.h>
#include <plotting/generators/transform.h>

namespace plotting
{
    struct LabelMaker: pipeline::Transform<LabelMaker>
    {
        template<typename B>
        LabelMaker(B&& b): base(FWD(b)) {}
        TextProperties base;

        Text operator()(std::tuple<agge::point_r const&, std::string const&> t) const
        {
            auto const& [position, label] = t;
            Text n(base);
            n.text(label);
            n.position(position);
            return n;
        }
    };

    inline agge::box_r estimate_box(LabelMaker const& labelMaker, std::string value = "0.0")
    {
        agge::richtext_t text({labelMaker.base._font,});
        text << value.c_str();
        auto& text_engine = plotting::global::get_text_engine();
        float const max_width = 1000.0f;
        auto wrap = agge::limit::wrap(max_width);
        return text_engine.measure(text, wrap);
    }
}

