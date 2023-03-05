#pragma once
#include <palgebra/algebra/rules.h>
#include <agge/utils/pod_vector.h>

namespace agge
{
    struct Distance
    {
        real_t distance = 0.0f;

        static Distance from(Vector_r v) { return Distance{norm(v)}; }
    };

    struct DistanceDirection
    {
        real_t distance  = 0.0f;
        real_t direction = 0.0f;

        static DistanceDirection from(Vector_r v) { return DistanceDirection{norm(v), atan2(v.y, v.x)}; }
    };

    using Path = pod_vector<Point_r>;

    template<typename E = Distance>
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

        void clear()               { data.clear(); }
        void push_back(E e)        { data.push_back(e); }
        void push_back(Vector_r v) { data.push_back(E::from(v)); }

        pod_vector<E> data;
    };

    template<typename E>
    struct PathAnnotationFeeder: rules::Consumer<PathAnnotationFeeder<E>>
    {
        PathAnnotationFeeder(Path& p, PathAnnotation<E>& a): path(p), annotation(a) {}

        pod_vector<Point_r>& path;
        PathAnnotation<E>&   annotation;

        void move_to(Point_r p)
        {
            annotation.clear();
            annotation.push_back(E{});
            path.clear();
            path.push_back(p);
        }

        void line_to(Point_r p)
        {
            if(path.empty())
                return move_to(p);
            annotation.push_back(p - path.back());
            path.push_back(p);
        }
    };

}