#pragma once

#include <agge/utils/vertex_sequence.h>
#include <agge/utils/static_any.h>
#include <agge/utils/tools.h>
#include <agge/primitives/algebra/rules.h>

namespace agge
{
    typedef pod_vector<Point_r> points;

    class stroke: public rules::PointGenerator<stroke>, vertex_sequence, noncopyable
    {
    public:
        struct cap;
        struct join;

    public:
        stroke();
        ~stroke();

        // Vertex population
        void remove_all();
        using vertex_sequence::move_to;
        using vertex_sequence::line_to;
        void close_polygon();
        void add_vertex(real_t x, real_t y, int command);

        // Vertex access
        int vertex(real_t* x, real_t* y);

        // Setup
        stroke& width(real_t w);

        template <typename CapT>
        stroke& set_cap(const CapT& c);

        template <typename JoinT>
        stroke& set_join(const JoinT& j);

    private:
        enum state {
            // Stages
            start_cap = 0x00,
            outline_forward = 0x01,
            outline_forward_closed = 0x02,
            end_poly1 = 0x03,
            end_cap = 0x04,
            outline_backward = 0x05,
            end_poly = 0x06,
            stop = 0x07,
            stage_mask = 0x07,

            // Flags
            closed = 0x10,
            moveto = 0x20,
            ready = 0x40
        };

    private:
        bool prepare();
        void set_state(int stage_and_flags);

    private:
        points _output;
        vertex_sequence::const_iterator _i;
        points::const_iterator _o;

        const cap*     _cap;
        const join*    _join;
        static_any<16> _capBuffer;
        static_any<16>  _joinBuffer;


        real_t _width;
        int    _state;
    };

    struct stroke::cap
    {
        virtual ~cap() { }
        virtual void calc(points& output, real_t w, const Point_r& v0, real_t d, const Point_r& v1) const = 0;
    };

    struct stroke::join
    {
        virtual ~join() { }
        virtual void calc(points& output, real_t w, const Point_r& v0, real_t d01, const Point_r& v1, real_t d12, const Point_r& v2) const = 0;
    };



    template <typename CapT>
    inline stroke& stroke::set_cap(const CapT& c)
    {
        _capBuffer = c;
        _cap = &_capBuffer.get<CapT>();
        return *this;
    }

    template <typename JoinT>
    inline stroke& stroke::set_join(const JoinT& j)
    {
        _joinBuffer = j;
        _join = &_joinBuffer.get<JoinT>();
        return *this;
    }
}
