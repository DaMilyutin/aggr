#pragma once

#include <plotting/types/rasteriser.h>
#include <plotting/common/font_loader.h>
#include <plotting/common/services.h>

#include <agge.text/text_engine.h>


#include <agge.text/layout.h>



namespace plotting
{
    using text_engine = agge::text_engine<rasterizer>;
    using layout = agge::layout;

    inline text_engine& get_text_engine()
    {
        static native_font_loader loader;
        static text_engine text_eng(loader);
        return text_eng;
    }


    inline layout& get_layout()
    {
        static layout lay;
        return lay;
    }
}