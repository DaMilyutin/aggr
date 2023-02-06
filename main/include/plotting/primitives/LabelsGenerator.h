#pragma once
#include <plotting/primitives/EntitiesGenerator.h>
#include <plotting/primitives/Text.h>

#include <functional>
#include <plotting/generators/Iota.h>
#include <plotting/generators/zip.h>



namespace plotting
{
    struct LabelData
    {
        agge::point_r  position;
        double         label;
    };

    //using ParallelLabelsGenerator = pipeline::zip_helper<PointsOnSegmentGenerator, InfiniteLinspace>;

    struct ParallelLabelsGenerator
    {
        pipeline::Iota<port_t, port_diff_t, size_t>        points;
        pipeline::Iota<double, double, pipeline::Infinite> labels;

        using Sentinel = pipeline::Iota<port_t, port_diff_t, size_t>::Sentinel;

        class Iterator
        {
        public:
            Iterator(ParallelLabelsGenerator const& ref)
                : _position(ref.points.begin()), _label(ref.labels.begin())
            {}

            LabelData operator*() const { return LabelData{*_position, *_label}; }

            Iterator& operator++()
            {
                ++_position;
                ++_label;
                return *this;
            }

            bool operator!=(Sentinel const& s) const { return _position != s; }
        private:
            pipeline::Iota<port_t, port_diff_t, size_t>::Iterator        _position;
            pipeline::Iota<double, double, pipeline::Infinite>::Iterator _label;
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
            n.text(formatter(l.label));
            n.position(l.position);
            return n;
        }
    };

    inline agge::box_r estimate_box(LabelMaker const& labelMaker, double value = 0.0)
    {
        agge::richtext_t text({labelMaker.base._font,});
        text << labelMaker.formatter(value).c_str();
        auto& text_engine = plotting::global::get_text_engine();
        float const max_width = 1000.0f;
        auto wrap = agge::limit::wrap(max_width);
        return text_engine.measure(text, wrap);
    }
}

