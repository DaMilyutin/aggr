#pragma once

#include <agge/utils/vertex_sequence.h>
#include <agge/primitives/algebra/rules.h>

namespace agge
{
    class dash: public rules::PointGenerator<dash>, private vertex_sequence
    {
    public:
        dash();

        // Setup
        dash& dash_start(real_t offset = 0.f); // clears and starts new
        dash& add_dash(real_t dash_length, real_t gap_length);


        // Vertex population
        void remove_all();
        using vertex_sequence::move_to;
        using vertex_sequence::line_to;
        void close_polygon();
        void add_vertex(real_t x, real_t y, int command);

        // Vertex access
        int vertex(real_t* x, real_t* y);

    private:
        enum state { initial, seek, move, emit_source, finish_dash, complete };

        struct dash_gap
        {
            real_t dash_length, gap_length;
        };

    private:
        void seek_dash_start();
        void interpolate_current(real_t* x, real_t* y) const;

    private:
        vertex_sequence::const_iterator _j;
        agge::pod_vector<dash_gap>::const_iterator _dash;
        real_t _t, _dash_length, _dash_start;
        Point_r _start;
        state _state;
        agge::pod_vector<dash_gap> _pattern;
    };
}
