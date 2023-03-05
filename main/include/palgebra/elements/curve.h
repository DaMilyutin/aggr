#pragma once
#include <palgebra/algebra/rules.h>
#include <agge/types/point.h>
#include <agge/math/bezier.h>

namespace agge
{
    namespace elements
    {
        // point generators for
        //  intermediary points

        template<typename Interp>
        class Curve: public rules::PointGenerator<Curve<Interp>>
        {
        public:
            Curve(Interp i, count_t count)
                : interp_(i), count_(count)
            {}

            struct Sentinel
            {};

            friend class Iterator;

            class Iterator
            {
            public:
                Iterator(Curve const& c)
                    : interp_(c.interp_), step_(1.0f/(c.count_)), count_(c.count_)
                {}

                Point_r operator*() const { return interp_.at(t_); }

                Iterator& operator++()
                {
                    --count_;
                    t_ += step_;
                    return *this;
                }

                bool operator==(Sentinel) const { return count_ == 0; }
                bool operator!=(Sentinel) const { return count_ != 0; }

            private:
                Interp const& interp_;
                real_t       t_     = 0.0f;
                real_t const step_  = 0.1f;
                count_t      count_ = 10;
            };

            Iterator begin() const { return Iterator(*this); }
            Sentinel end()   const { return {}; }

        private:
            Interp  interp_;
            count_t count_;
        };
    }
}