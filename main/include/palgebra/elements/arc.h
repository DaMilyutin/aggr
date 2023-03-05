#pragma once
#include <palgebra/algebra/rules.h>
#include <agge/types/point.h>

namespace agge
{
    namespace elements
    {
        // point generators for
        //  intermediary points

        class Arc: public rules::PointGenerator<Arc>
        {
            static inline real_t optimal_step(real_t radius)
            {
                real_t const x = (1 - error / radius);
                return acosf(2.0f*x*x  - 1);
            }
        public:
            static constexpr real_t error = 1.0f;

            Arc(Point_r start, Vector_r inc, Vector_r rot, count_t count)
                : start_(start), inc_(inc), rot_(rot), count_(count)
            {}

            Arc(Point_r cen, real_t radius, real_t a1, real_t a2)
            {
                real_t const angle = a2 - a1;
                real_t const granularity = optimal_step(radius);
                count_ = count_t(angle*radius/granularity);
                if(count_ == 0)
                    return;
                real_t const astep = angle/count_;
                rot_.x = cosf(astep);
                rot_.y = sinf(astep);
                real_t const inc_norm = 2.0f*radius*sinf(0.5f*astep);
                a1 -= astep*0.5f;
                //if(angle < 0) a1 += agge::pi;
                Vector_r v1 = {-sinf(a1), cosf(a1)};
                inc_ = v1*inc_norm;
                start_.x = cen.x + v1.y*radius;
                start_.y = cen.y - v1.x*radius;
                ++count_;
            }

            Arc(Point_r cen, real_t radius, real_t a1, real_t a2, count_t count)
                : count_(count)
            {
                if(count == 0)
                    return;
                start_.x = cen.x + cosf(a1)*radius;
                start_.y = cen.y + sinf(a1)*radius;

                real_t const angle = a2 - a1;
                real_t const astep = angle/count;
                rot_.x = cosf(astep);
                rot_.y = sinf(astep);

                real_t half_astep = 0.5f*astep;
                real_t const inc_norm = 2.0f*radius*sinf(half_astep);
                a1 += half_astep + agge::pi*0.5f;
                if(angle < 0) a1 += agge::pi;
                Vector_r v1 = {cosf(a1), sinf(a1)};
                inc_ = v1*inc_norm;

                ++count_;
            }

            struct Sentinel
            {};

            friend class Iterator;

            class Iterator
            {
            public:
                Iterator(Arc const& a)
                    : curr(a.start_), inc(a.inc_), rot(a.rot_), count(a.count_)
                {}

                Point_r const& operator*() const { return curr; }

                Iterator& operator++()
                {
                    --count;
                    curr += inc;
                    inc = multiply(inc, rot);
                    return *this;
                }

                bool operator==(Sentinel) const { return count == 0; }
                bool operator!=(Sentinel) const { return count != 0; }

            private:
                Point_r  curr;
                Vector_r inc;
                Vector_r const rot;
                count_t  count;
            };

            Iterator begin() const { return Iterator(*this); }
            Sentinel end()   const { return {}; }

        private:
            Point_r  start_;
            Vector_r inc_;
            Vector_r rot_;
            int      count_ = 0;
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