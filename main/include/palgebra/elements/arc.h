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
            static inline real_t optimal_step(real_t radius, real_t error)
            {
                real_t const x = (1 - error / radius);
                return acosf(2.0f*x*x  - 1);
            }
        public:
            static constexpr real_t default_error = 1.0f;

            struct Natural
            {
                Point_r center;
                real_t  radius;
                real_t  angle1;
                real_t  angle2;
            };

            Arc(Point_r cen, real_t radius, real_t a1, real_t a2, real_t error = default_error)
                : natural{cen, radius, a1, a2}
            {
                real_t const angle = natural.angle2 - natural.angle1;
                real_t const granularity = optimal_step(radius, error);
                count_ = count_t(fabsf(angle*radius)/granularity);
            }

            Arc(count_t count, Point_r cen, real_t radius, real_t a1, real_t a2)
                : natural{cen, radius, a1, a2}, count_(count)
            {}

            struct sentinel
            {};

            friend class const_iterator;

            class const_iterator
            {
            public:
                const_iterator(Natural const& n, count_t count)
                    : count_(count+1)
                {
                    real_t const angle = n.angle2 - n.angle1;
                    real_t const astep = angle/count;
                    rot_.x = cosf(astep);
                    rot_.y = sinf(astep);
                    real_t const inc_norm = 2.0f*n.radius*sinf(0.5f*astep);
                    real_t a1 = n.angle1 + astep*0.5f;
                    //if(angle < 0) a1 += agge::pi;
                    Vector_r v1 = {-sinf(a1), cosf(a1)};
                    inc_ = v1*inc_norm;
                    curr_.x = n.center.x + cosf(n.angle1)*n.radius;
                    curr_.y = n.center.y + sinf(n.angle1)*n.radius;
                }

                Point_r const& operator*() const { return curr_; }

                const_iterator& operator++()
                {
                    --count_;
                    curr_ += inc_;
                    inc_ = multiply(inc_, rot_);
                    return *this;
                }

                bool operator==(sentinel) const { return count_ == 0; }
                bool operator!=(sentinel) const { return count_ != 0; }

            private:
                Point_r  curr_;
                Vector_r inc_;
                Vector_r rot_;
                count_t  count_;
            };

            Arc& dropEnds()
            {
                if(count_ > 1)
                {
                    count_ -= 2;
                    real_t const astep = (natural.angle2 - natural.angle1)/count_;
                    natural.angle1 += astep;
                    natural.angle2 -= astep;
                }
                return *this;
            }

            Arc& reverse()
            {
                if(count_ > 1)
                {
                    real_t const tmp = natural.angle1;
                    natural.angle1 = natural.angle2;
                    natural.angle2 = tmp;
                }
                return *this;
            }

            const_iterator begin() const { return const_iterator(natural, count_); }
            sentinel end()   const { return {}; }

        private:
            Natural  natural;
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