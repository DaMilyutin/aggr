#pragma once
#include <vector>
#include <functional>

#include <agge/primitives/pipeline.h>
#include <agge/primitives/polyline.h>

#include <plotting/generators/PointsGen.h>

namespace plotting
{
    template<typename E>
    agge::polyline& operator<<(agge::polyline& ret, pipeline::PointsGenerator<E, port_t> const& gen)
    {
        for(auto&& p: gen._get_())
            ret.line_to(p);
        return ret;
    }

    template<typename E>
    agge::polyline& operator<<(agge::polyline& ret, pipeline::PointsGenerator<E, port_t>&& gen)
    {
        for(auto&& p: gen._get_())
            ret.line_to(p);
        return ret;
    }

    struct Function: public pipeline::PointsGenerator<Function, repr_t>
    {
        double a    =-5.;
        double b    = 5.;
        double step = 0.1;

        int counts() const { return int(floor((b-a)/step)); }

        class Iterator
        {
        public:
            Iterator(double t, Function const& ref): t(t), ref(ref) {}

            Iterator& operator++() { t+= ref.step; return *this; }
            repr_t operator*() const { return {t, ref.f(t)}; }
            bool operator != (Iterator const& rhs) const { return t <= rhs.t; }
        private:
            double          t;
            Function const& ref;
        };

        Iterator begin() const { return {a, *this}; }
        Iterator end() const   { return {b, *this}; }

        std::function<double(double)> f;
    };

    struct ChartData: public pipeline::PointsGenerator<ChartData, repr_t>
    {
        std::vector<repr_t> data;

        void from(Function const& f)
        {
            data.reserve(f.counts());
            data.clear();
            for(auto&& p: f)
                data.emplace_back(std::move(p));
        }

        auto begin() const { return std::begin(data); }
        auto end() const { return std::end(data); }
    };


}