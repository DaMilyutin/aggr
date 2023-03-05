#pragma once
#include <palgebra/algebra/rules.h>
#include <palgebra/mediators/Path.h>

namespace agge
{
    namespace elements
    {
        // point generators for
        //  intermediary points

        class Vertex: public rules::PointGenerator<Vertex>
        {
        public:
            static constexpr real_t granularity = 5.0f;

            Vertex(Point_r p)
                : point(p)
            {}

            struct Sentinel
            {};

            friend class Iterator;

            class Iterator
            {
            public:
                Iterator(Point_r const& p)
                    : point(&p)
                {}

                Point_r const& operator*() const { return *point; }

                Iterator& operator++()
                {
                    point = 0;
                    return *this;
                }

                bool operator==(Sentinel) const { return point == 0; }
                bool operator!=(Sentinel) const { return point != 0; }

            private:
                Point_r const* point;
            };

            Iterator begin() const { return Iterator(point); }
            Sentinel end()   const { return {}; }

        private:
            Point_r point;
        };


        //Arc(Point_r const& prev, real_t dist_prev, Point_r const& curr1,
        //    Point_r const& curr2, real_t dist_next, Point_r const& next)
        //{
        //    Vector_r const v1 = (1.f/dist_prev)*(curr1 - prev);
        //    Vector_r const v2 = (1.f/dist_next)*(next  - curr2);
        //    Vector_r const A = dotcross(v1, v2);
        //    real_t const angle = atan2(A.y, A.x);
        //    if(fabsf(angle) < 1.e-2f)
        //        return;
        //    real_t const radius = 0.5f*norm(curr2 - curr1)/sinf(0.5f*angle);
        //    count = int(angle*radius/granularity);
        //    if(count == 0)
        //        return;
        //    real_t const astep = angle/count;
        //    rot.x = cosf(astep);
        //    rot.y = sinf(astep);
        //    real_t const inc_norm = 2.0f*radius*sinf(0.5f*astep);
        //    inc = multiply(v1, rot)*inc_norm;
        //}

        //Arc(Point_r const& prev, real_t dist_prev, Point_r const& curr1,
        //    Point_r const& curr2, real_t dist_next, Point_r const& next,
        //    int theCount)
        //{
        //    Vector_r const v1 = (1.f/dist_prev)*(curr1 - prev);
        //    Vector_r const v2 = (1.f/dist_next)*(next  - curr2);
        //    Vector_r const A = dotcross(v1, v2);
        //    real_t const angle = atan2(A.y, A.x);
        //    if(fabsf(angle) < 1.e-2f)
        //    {
        //        count = 0;
        //        return;
        //    }

        //    real_t const radius = 0.5f*norm(curr2 - curr1)/sinf(0.5f*angle);

        //    count = theCount;
        //    real_t const astep = angle/count;
        //    rot.x = cosf(astep);
        //    rot.y = sinf(astep);
        //    real_t const inc_norm = 2.0f*radius*sinf(0.5f*astep);
        //    inc = multiply(v1, rot)*inc_norm;
        //}
    }
}