#pragma once
#include <vector>
#include <functional>

#include <agge/primitives/pipeline.h>
#include <agge/primitives/polyline.h>

#include <plotting/CoordinateSystem.h>

namespace plotting
{
    namespace pipeline
    {
        template<typename E>
        struct PointGenerator: public agge::pipeline::terminal<E>
        {};

        template<typename G>
        struct transformed
        {
            CoordinateSystem const& coordinates;
            G                       generator;
        };
    }

    template<typename E>
    pipeline::transformed<E const&> operator/(pipeline::PointGenerator<E> const& points, CoordinateSystem const& c)
    {
        return {c, points._get_()};
    }

    template<typename E>
    pipeline::transformed<E&&> operator/(pipeline::PointGenerator<E>&& points, CoordinateSystem const& c)
    {
        return {c, std::move(points._get_())};
    }

    template<typename E>
    agge::polyline& operator<<(agge::polyline& ret, pipeline::transformed<E> const& t)
    {
        for(auto&& p: t.generator)
            ret.line_to(t.coordinates << p);
        return ret;
    }

    template<typename E>
    agge::polyline& operator<<(agge::polyline& ret, pipeline::transformed<E>&& t)
    {
        for(auto&& p: t.generator)
            ret.line_to(p/t.coordinates);
        return ret;
    }

    template<typename E>
    agge::polyline&& operator<<(agge::polyline&& ret, pipeline::transformed<E> const& t)
    {
        for(auto&& p: t.generator)
            ret.line_to(t.coordinates/p);
        return ret;
    }

    template<typename E>
    agge::polyline&& operator<<(agge::polyline&& ret, pipeline::transformed<E>&& t)
    {
        for(auto&& p: t.generator)
            ret.line_to(p/t.coordinates);
        return ret;
    }

    struct Function: pipeline::PointGenerator<Function>
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
            bool operator != (Iterator const& rhs) { return t <= rhs.t; }
        private:
            double          t;
            Function const& ref;
        };

        Iterator begin() const { return {a, *this}; }
        Iterator end() const   { return {b, *this}; }

        std::function<double(double)> f;
    };

    struct ChartData: pipeline::PointGenerator<ChartData>
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