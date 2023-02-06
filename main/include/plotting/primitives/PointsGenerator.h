#pragma once
#include <utility>

#include <plotting/CoordinateSystem.h>


namespace plotting
{
    //namespace pipeline
    //{
    //    template<typename E, typename PointT>
    //    struct PointsGenerator: public agge::pipeline::terminal<E>
    //    {};

    //    template<typename G>
    //    class Transformed: public PointsGenerator<Transformed<G>, port_t>
    //    {
    //        using UnderlyingIterator = std::remove_cv_t<decltype(std::begin(std::declval<G>()))>;
    //        using UnderlyingSentinel = std::remove_cv_t<decltype(std::end(std::declval<G>()))>;

    //        using from_t = std::remove_cvref_t<decltype(*std::declval<UnderlyingIterator>())>;
    //        static_assert(std::is_same_v<from_t, repr_t>,
    //                      "generator from must generate repr_t or ref of repr_t");

    //    public:
    //        using Sentinel = UnderlyingSentinel;

    //        class Iterator
    //        {
    //        protected:
    //            friend class Transformed;
    //            Iterator(Transformed const& t)
    //                : _it(t.generator.begin())
    //                , _repr2port(t.repr2port)
    //            {}

    //            UnderlyingIterator _it;
    //            ReprToPort const& _repr2port;

    //        public:

    //            Iterator& operator++() { ++_it; return *this; }
    //            port_t operator*() const { return *_it/_repr2port; }
    //            bool operator!=(Sentinel const& s) const { return _it != s; }
    //        };

    //        Iterator begin() const { return Iterator(*this); }
    //        Sentinel end() const   { return Sentinel(generator.end()); }

    //        G                 generator;
    //        ReprToPort const& repr2port;
    //    };


    //    template<typename G>
    //    Transformed<G> operator/(PointsGenerator<G, repr_t> const& g, ReprToPort const& c)
    //    {
    //        return {{}, g._get_(), c};
    //    }

    //    template<typename G>
    //    Transformed<G> operator/(PointsGenerator<G, repr_t>&& g, ReprToPort const& c)
    //    {
    //        return {{}, std::move(g._get_()), c};
    //    }
    //}

    struct PointsOnSegmentGenerator: pipeline::Generator<PointsOnSegmentGenerator>
    {
        agge::point_r  initial{};
        agge::vector_r increment{};
        int            number{};

        class Sentinel
        {};

        class Iterator
        {
            friend struct PointsOnSegmentGenerator;
            Iterator(PointsOnSegmentGenerator const& ref)
                : point(ref.initial), i(ref.number), ref(ref)
            {}
        public:
            Iterator& operator++()
            {
                --i;
                point += ref.increment;
                return *this;
            }
            agge::point_r const& operator*() const { return point; }

            bool operator!=(Sentinel) const { return i > 0; }
            bool operator==(Sentinel) const { return i == 0; }
        private:
            agge::point_r          point;
            int                    i;
            PointsOnSegmentGenerator const& ref;
        };
        Iterator begin() const { return Iterator(*this); }
        Sentinel end()   const { return {}; }
    };

}