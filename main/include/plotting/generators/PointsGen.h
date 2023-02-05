#pragma once
#include <utility>

#include <plotting/CoordinateSystem.h>


namespace plotting
{
    namespace pipeline
    {
        template<typename E, typename PointT>
        struct PointsGenerator: public agge::pipeline::terminal<E>
        {};

        template<typename G>
        class Transformed: public PointsGenerator<Transformed<G>, port_t>
        {
            using UnderlyingIterator = std::remove_cv_t<decltype(std::begin(std::declval<G>()))>;
            using UnderlyingSentinel = std::remove_cv_t<decltype(std::end(std::declval<G>()))>;

            using Underlying = decltype(std::begin(std::declval<G>()));
            using from_t = std::remove_cv_t<decltype(*std::begin(std::declval<G>()))>;
            static_assert(std::is_same_v<from_t, repr_t>,
                          "generator from must generate repr_t or ref of repr_t");

        public:
            using Sentinel = UnderlyingSentinel;

            class Iterator
            {
            protected:
                friend class Transformed;
                Iterator(Transformed const& t)
                    : _it(t.generator.begin())
                    , _coordinates(t.coordinates)
                {}

                UnderlyingIterator _it;
                CoordinateSystem const& _coordinates;

            public:

                Iterator& operator++() { ++_it; return *this; }
                port_t operator*() const { return *_it/_coordinates; }
                bool operator!=(Sentinel const& s) const { return _it != s; }
            };

            Iterator begin() const { return Iterator(*this); }
            Sentinel end() const   { return Sentinel(generator.end()); }

            G                       generator;
            CoordinateSystem const& coordinates;
        };
    }

    template<typename G>
    pipeline::Transformed<G> operator/(pipeline::PointsGenerator<G, repr_t> const& g, CoordinateSystem const& c)
    {
        return {{}, g._get_(), c};
    }

    template<typename G>
    pipeline::Transformed<G> operator/(pipeline::PointsGenerator<G, repr_t>&& g, CoordinateSystem const& c)
    {
        return {{}, std::move(g._get_()), c};
    }

    struct Selector_Any
    {
        bool operator()(int) const { return true; }
    };

    struct Selector_SkipOverPeriod
    {
        int period = 10;
        int offset = 0;

        bool operator()(int x) const
        {
            return (x-offset)%period != 0;
        }
    };

    template<typename Selector = Selector_Any>
    struct PointsOnSegmentGenerator: pipeline::PointsGenerator<PointsOnSegmentGenerator<Selector>, agge::point_r>
    {
        agge::point_r  initial{};
        agge::vector_r increment{};
        int            start{};
        int            number{};
        Selector       select{};

        class Sentinel
        {
        public:
            int const number;
        };

        class Iterator
        {
        public:
            Iterator(PointsOnSegmentGenerator const& ref)
                : point(ref.initial), i(ref.start), ref(ref)
            {
                next_sel();
            }

            agge::point_r const& operator*() const { return point; }

            Iterator& operator++()
            {
                skip();
                next_sel();
                return *this;
            }

            bool operator!=(Sentinel const& s) const { return i < s.number; }
        private:
            void skip()
            {
                ++i;
                point += ref.increment;
            }

            void next_sel()
            {
                while(!ref.select(i))
                    skip();
            }

            agge::point_r          point;
            int                    i;
            PointsOnSegmentGenerator const& ref;
        };
        Iterator begin() const { return Iterator(*this); }
        Sentinel end()   const { return {number}; }
    };

}