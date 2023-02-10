#pragma once
#include <plotting/piping.h>

#include <agge/primitives/polyline.h>

namespace plotting
{
    namespace filters
    {
        struct FarEnough: public piping::Filter<FarEnough>
        {
            FarEnough(agge::real_t eps): eps(eps) {}

            bool operator()(agge::point_r const& cur) const
            {
                bool const far = fabs(cur.x - prev.x) + fabs(cur.y - prev.y) > eps;
                if(far)
                    prev = cur;
                return far;
            }

            bool operator()(agge::polyline::Item const& cur) const
            {
                bool const far = fabs(cur.point.x - prev.x) + fabs(cur.point.y - prev.y) > eps;
                if(far)
                    prev = cur.point;
                return far;
            }

            agge::real_t  const   eps{0.5f};
            agge::point_r mutable prev{-std::numeric_limits<agge::real_t>::infinity(),
                                       -std::numeric_limits<agge::real_t>::infinity()};
        };

        struct FarEnough_TimeSeries: public piping::Filter<FarEnough_TimeSeries>
        {
            FarEnough_TimeSeries(agge::real_t eps): eps(eps) {}

            bool operator()(agge::point_r const& cur) const
            {
                bool far = fabs(cur.x - prev.x) + fabs(cur.y - prev.y) > eps;
                if(far)
                    prev = cur;
                return far;
            }

            bool operator()(agge::polyline::Item const& cur) const
            {
                bool const far = fabs(cur.point.x - prev.x) + fabs(cur.point.y - prev.y) > eps;
                if(far)
                    prev = cur.point;
                return far;
            }

            agge::real_t  const   eps{0.5f};
            agge::point_r mutable prev{-std::numeric_limits<agge::real_t>::infinity(),
                                       -std::numeric_limits<agge::real_t>::infinity()};
        };

        struct SkipOverPeriod: public piping::Filter<SkipOverPeriod>
        {
            SkipOverPeriod(int p, int o = 0): period(p), offset(o) {}
            SkipOverPeriod(SkipOverPeriod&&) = default;
            SkipOverPeriod(SkipOverPeriod const&) = default;

            int const   period = 10;
            int mutable offset = 0;

            bool operator()(auto const&) const
            {
                bool keep = offset != 0;
                if(++offset >= period)
                    offset = 0;
                return keep;
            }
        };
    }
}