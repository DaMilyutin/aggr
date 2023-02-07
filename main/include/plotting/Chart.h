#pragma once
#include <vector>
#include <functional>

#include <agge/primitives/pipeline.h>
#include <agge/primitives/polyline.h>

#include <plotting/generators/generator.h>
#include <plotting/generators/iota.h>
#include <plotting/generators/transform.h>

namespace plotting
{
    template<typename E>
    agge::polyline& operator<<(agge::polyline& ret, pipeline::Generator<E> const& gen)
    {
        for(auto&& p: gen._get_())
            ret.line_to(p);
        return ret;
    }

    template<typename E>
    agge::polyline& operator<<(agge::polyline& ret, pipeline::Generator<E>&& gen)
    {
        for(auto&& p: gen._get_())
            ret.line_to(p);
        return ret;
    }

    struct FunctionY: public pipeline::Transformer<FunctionY>
    {
        template<typename F>
        FunctionY(F&& f): funcion(FWD(f)) {}

        repr_t operator()(double x) const { return {x, funcion(x)}; }
        std::function<double(double)> funcion;
    };

    struct FunctionX: public pipeline::Transformer<FunctionX>
    {
        template<typename F>
        FunctionX(F&& f): funcion(FWD(f)) {}

        repr_t operator()(double y) const { return {funcion(y), y}; }
        std::function<double(double)> funcion;
    };

    struct ChartData: public pipeline::Generator<ChartData>
    {
        std::vector<repr_t> data;

        template<typename G>
        void from(pipeline::Generator<G> const& g)
        {
            data.clear();
            for(auto&& p: g._get_())
                data.emplace_back(std::move(p));
        }

        auto begin() const { return std::begin(data); }
        auto end() const { return std::end(data); }
    };


}