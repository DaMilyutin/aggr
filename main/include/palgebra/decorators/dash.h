#pragma once
#include <palgebra/algebra/rules.h>
#include <agge/utils/vertex_sequence.h>

namespace agge
{
    class Dash: public rules::PointGenerator<Dash>, private vertex_sequence
    {
    public:
        Dash& dash_start(real_t offset)
        {
            pattern_.clear();
            dash_start_ = offset;
            return *this;
        }

        Dash& add_dash(real_t dash_length, real_t gap_length)
        {
            dash_gap dg = {dash_length, gap_length};
            pattern_.push_back(dg);
            return *this;
        }



    private:
        struct dash_gap
        {
            real_t dash_length;
            real_t gap_length;
        };
        real_t                     dash_start_ = 0.f;
        agge::pod_vector<dash_gap> pattern_;
    };
}