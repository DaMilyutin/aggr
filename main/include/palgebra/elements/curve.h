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
        class Curve: public rules::Yield<Curve<Interp>>
        {
        public:
            Curve(Interp i, count_t count)
                : interp_(i), count_(count)
            {}

            struct sentinel
            {};

            friend class const_iterator;

            class const_iterator
            {
            public:
                const_iterator(Curve const& c)
                    : interp_(c.interp_), step_(1.0f/(c.count_)), count_(c.count_)
                {}

                Point_r operator*() const { return interp_.at(t_); }

                const_iterator& operator++()
                {
                    --count_;
                    t_ += step_;
                    return *this;
                }

                bool operator==(sentinel) const { return count_ == 0; }
                bool operator!=(sentinel) const { return count_ != 0; }

            private:
                Interp const& interp_;
                real_t        t_     = 0.0f;
                real_t const  step_  = 0.1f;
                count_t       count_ = 10;
            };

            const_iterator begin() const { return const_iterator(*this); }
            sentinel       end()   const { return {}; }

            count_t size() const { return count_ + 1; }
        private:
            Interp  interp_;
            count_t count_;
        };
    }
}