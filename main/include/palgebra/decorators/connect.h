#pragma once
#include <palgebra/algebra/rules.h>
#include <palgebra/mediators/Path.h>

namespace agge
{
    namespace connectors
    {
        // point generators for
        //  intermediary points

        struct Bevel // simple line connector does nothing
        {
            Bevel() = default;
            Bevel(Point_r const& prev, real_t dist_prev, Point_r const& curr1,
                  Point_r const& curr2, real_t dist_next, Point_r const& next)
            {}

            struct Iterator
            {
                bool operator!= () const { return false; }
                bool operator== () const { return true; }
            };

            void begin() const { return {}; }
            void end()   const { return {}; }
        };


    }
}