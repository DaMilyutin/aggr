#pragma once
#include <agge/color.h>
#include <agge/primitives/figures.h>
#include <agge/primitives/dash.h>
#include <agge/primitives/stroke.h>
#include <agge/primitives/stroke_features.h>

#include <agge.text/layout.h>
#include <agge.text/font.h>
#include <agge.text/glyph.h>


#include <plotting/Canvas.h>
#include <plotting/types/text_engine.h>


namespace plotting
{
    template<typename Generator, typename Maker>
    class LabelsGenerator
    {
    public:
        Generator   gen;
        Maker       maker;
        agge::color color;
    };
}