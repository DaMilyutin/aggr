#pragma once
#include <vector>
#include <functional>

#include <agge/primitives/pipeline.h>
#include <agge/primitives/polyline.h>

#include <plotting/piping/rules.h>

namespace plotting
{
    template<typename E>
    agge::polyline& operator<<(agge::polyline& ret, piping::Yield<E> const& gen)
    {
        for(auto&& p: gen._get_())
            ret << p;
        return ret;
    }

    template<typename E>
    agge::polyline& operator<<(agge::polyline& ret, piping::Yield<E>&& gen)
    {
        for(auto&& p: gen._get_())
            ret << p;
        return ret;
    }

    struct FunctionY: public piping::Transform<FunctionY>
    {
        template<typename F>
        FunctionY(F&& f): Y(FWD(f)) {}

        repr_t operator()(double t) const { return {t, Y(t)}; }
        std::function<double(double)> Y;
    };

    struct FunctionX: public piping::Transform<FunctionX>
    {
        template<typename F>
        FunctionX(F&& f): X(FWD(f)) {}

        repr_t operator()(double t) const { return {X(t), t}; }
        std::function<double(double)> X;
    };

    struct FunctionXY: public piping::Transform<FunctionXY>
    {
        template<typename F, typename G>
        FunctionXY(F&& f, G&& g): X(FWD(f)), Y(FWD(g)) {}

        repr_t operator()(double t) const { return {X(t), Y(t)}; }
        std::function<double(double)> X;
        std::function<double(double)> Y;
    };

    struct ChartData: public piping::Yield<ChartData>
    {
        std::vector<repr_t> data;

        template<typename G>
        void operator<<(piping::Yield<G> const& g)
        {
            data.clear();
            for(auto&& p: g._get_())
                data.emplace_back(std::move(p));
        }

        auto begin() const { return std::begin(data); }
        auto end() const { return std::end(data); }
    };
}