#pragma once
#include <palgebra/algebra/rules.h>
#include <agge/utils/pod_vector.h>

namespace agge
{
    using Path = pod_vector<Point_r>;

    namespace annotations
    {
        struct Distance
        {
            real_t distance = 0.0f;

            static Distance from(Vector_r v) { return Distance{norm(v)}; }
        };

        struct DistanceDirection
        {
            real_t distance = 0.0f;
            real_t direction = 0.0f;

            static DistanceDirection from(Vector_r v) { return DistanceDirection{norm(v), atan2(v.y, v.x)}; }
        };

        template<typename S, typename A>
        struct Annotated
        {
            S segment;
            A annotation;
        };

        template<typename E>
        class PathAnnotation
        {
        public:
            PathAnnotation() = default;
            PathAnnotation(PathAnnotation const&) = default;
            PathAnnotation(PathAnnotation&&) = default;

            PathAnnotation(Path const& p)
            {
                data.resize(p.size());
                if(data.size() == 0)
                    return;
                data[0] = E{};
                for(agge::count_t i = 1; i < p.size(); ++i)
                    data[i] = E::from(p[i] - p[i-1]);
            }

            void clear() { data.clear(); }
            void push_back(E e) { data.push_back(e); }
            void push_back(Vector_r v) { data.push_back(E::from(v)); }

            pod_vector<E> data;
        };
    }
}