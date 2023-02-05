#pragma once
#include <plotting/generators/EntitiesGen.h>
#include <plotting/primitives/Text.h>
#include <functional>

#include "PointsGen.h"

namespace plotting
{
    struct LabelData
    {
        agge::point_r position;
        double        value;
    };

    struct ParallelLabelsGenerator
    {
        PointsOnSegmentGenerator<> points;
        double                     value_initial;
        double                     value_increment;

        using Sentinel = PointsOnSegmentGenerator<>::Sentinel;

        class Iterator
        {
        public:
            Iterator(ParallelLabelsGenerator const& ref)
                : _it(ref.points.begin()), _value(ref.value_initial), ref(ref)
            {}

            LabelData operator*() const { return LabelData{*_it, _value}; }

            Iterator& operator++()
            {
                ++_it;
                _value += ref.value_increment;
                return *this;
            }

            bool operator!=(Sentinel const& s) const { return _it != s; }
        private:
            PointsOnSegmentGenerator<>::Iterator _it;
            double                               _value;
            ParallelLabelsGenerator const&       ref;
        };
        Iterator begin() const { return Iterator(*this); }
        Sentinel end()   const { return points.end(); }
    };

    struct LabelMaker
    {
        std::function<std::string(double)> formatter = [](double x) { return std::to_string(x); };
        TextProperties base;
        Text operator()(LabelData const& l) const
        {
            Text n(base);
            n.text(formatter(l.value));
            n.position(l.position);
            return n;
        }
    };

    inline agge::box_r estimate_box(LabelMaker const& labelMaker, double initial = 0.0)
    {
        agge::richtext_t text({labelMaker.base._font,});
        text << labelMaker.formatter(initial).c_str();
        auto& text_engine = plotting::global::get_text_engine();
        float const max_width = 1000.0f;
        auto wrap = agge::limit::wrap(max_width);
        return text_engine.measure(text, wrap);
    }
}

