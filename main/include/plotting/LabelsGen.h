#pragma once
#include "EntitiesGen.h"
#include "Text.h"
#include <functional>

namespace plotting
{
    struct LabelData
    {
        agge::point_r position;
        double        value;
    };

    struct ParallelLabelsGenerator
    {
        LabelData      initial{};
        agge::vector_r direction{};
        double         inc{};
        int            start{};
        int            number{};

        class Sentinel
        {
        public:
            int const number;
        };

        class Iterator
        {
        public:
            Iterator(ParallelLabelsGenerator const& ref)
                : data(ref.initial), i(ref.start), ref(ref)
            {}

            LabelData const& operator*() const { return data; }

            Iterator& operator++()
            {
                skip();
                return *this;
            }

            bool operator!=(Sentinel const& s) const { return i < s.number; }
        private:
            void skip()
            {
                ++i;
                data.position += ref.direction;
                data.value += ref.inc;
            }

            LabelData                      data;
            int                            i;
            ParallelLabelsGenerator const& ref;
        };
        Iterator begin() const { return Iterator(*this); }
        Sentinel end()   const { return {number}; }
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


    template<typename S, typename Rn, typename Rs>
    plotting::Canvas<S, Rn, Rs>& operator<<(plotting::Canvas<S, Rn, Rs>& c,
        EntitiesGenerator<ParallelLabelsGenerator, LabelMaker> const& entities)
    {
        c << reset;
        for(auto const& l : entities.gen)
        {
            auto e = entities.maker(l);
            c << e << e._color;
        }
        return c;
    }
}

