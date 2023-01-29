//----------------------------------------------------------------------------
// Anti-Grain Geometry - Version 2.4
// Copyright (C) 2002-2005 Maxim Shemanarev (http://www.antigrain.com)
//
// Permission to copy, use, modify, sell and distribute this software 
// is granted provided this copyright notice appears in all copies. 
// This software is provided "as is" without express or implied
// warranty, and with no claim as to its suitability for any purpose.
//
//----------------------------------------------------------------------------
// Contact: mcseem@antigrain.com
//          mcseemagg@yahoo.com
//          http://www.antigrain.com
//----------------------------------------------------------------------------
//
// class ellipse
//
//----------------------------------------------------------------------------

#ifndef AGG_ELLIPSE_INCLUDED
#define AGG_ELLIPSE_INCLUDED

#include "paths.h"

namespace agg
{

    //----------------------------------------------------------------ellipse
    class ellipse
    {
    public:
        ellipse() : 
            m_x(0.0), m_y(0.0), m_rx(1.0), m_ry(1.0), m_scale(1.0), 
            m_num(4), m_step(0), m_cw(false) {}

        ellipse(agge::real_t x, agge::real_t y, agge::real_t rx, agge::real_t ry, 
                unsigned num_steps=0, bool cw=false) :
            m_x(x), m_y(y), m_rx(rx), m_ry(ry), m_scale(1.0), 
            m_num(num_steps), m_step(0), m_cw(cw) 
        {
            if(m_num == 0) calc_num_steps();
        }

        void init(agge::real_t x, agge::real_t y, agge::real_t rx, agge::real_t ry, 
                  unsigned num_steps=0, bool cw=false);

        void approximation_scale(agge::real_t scale);
        void rewind(unsigned path_id);
        unsigned vertex(agge::real_t* x, agge::real_t* y);

    private:
        void calc_num_steps();

        agge::real_t m_x;
        agge::real_t m_y;
        agge::real_t m_rx;
        agge::real_t m_ry;
        agge::real_t m_scale;
        unsigned m_num;
        unsigned m_step;
        bool m_cw;
    };

    //------------------------------------------------------------------------
    inline void ellipse::init(agge::real_t x, agge::real_t y, agge::real_t rx, agge::real_t ry, 
                              unsigned num_steps, bool cw)
    {
        m_x = x;
        m_y = y;
        m_rx = rx;
        m_ry = ry;
        m_num = num_steps;
        m_step = 0;
        m_cw = cw;
        if(m_num == 0) calc_num_steps();
    }

    //------------------------------------------------------------------------
    inline void ellipse::approximation_scale(agge::real_t scale)
    {   
        m_scale = scale;
        calc_num_steps();
    }

    //------------------------------------------------------------------------
    inline void ellipse::calc_num_steps()
    {
        agge::real_t ra = (common::fabs(m_rx) + common::fabs(m_ry)) / 2;
        agge::real_t da = common::acos(ra / (ra + 0.125f / m_scale)) * 2.0f;
        m_num = static_cast<int>(2*agge::pi / da);
    }

    //------------------------------------------------------------------------
    inline void ellipse::rewind(unsigned)
    {
        m_step = 0;
    }

    //------------------------------------------------------------------------
    inline unsigned ellipse::vertex(agge::real_t* x, agge::real_t* y)
    {
        if(m_step == m_num) 
        {
            ++m_step;
            return agge::path_command_end_poly | agge::path_flag_close;
        }
        if(m_step > m_num) return agge::path_command_stop;
        agge::real_t angle = agge::real_t(m_step) / agge::real_t(m_num) * 2.0f * agge::pi;
        if(m_cw) angle = 2.0f * agge::pi - angle;
        *x = m_x + agge::cos(angle) * m_rx;
        *y = m_y + agge::sin(angle) * m_ry;
        m_step++;
        return ((m_step == 1) ? agge::path_command_move_to : agge::path_command_line_to);
    }

}



#endif


