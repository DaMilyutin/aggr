#pragma once
#include <palgebra/algebra/rules.h>
#include <agge/utils/pod_vector.h>

namespace agge
{

    class Path
    {
    public:
        Path() = default;
        Path(Path const&) = default;
        Path(Path&&) = default;

        Path(pod_vector<Point_r>&& p): point_(std::move(p))
        {
            fillDistances();
        }

        Path(pod_vector<Point_r> const& p): point_(p)
        {
            fillDistances();
        }

        pod_vector<Point_r> const& points()    const { return point_; }
        pod_vector<real_t> const&  distances() const { return distance_; }

        void clear() { point_.clear(); distance_.clear(); }
        void push_back(Point_r p)
        {
            if(distance_.empty())
                distance_.push_back(0.0f);
            else
                distance_.push_back(distance(p, point_.back()));
            point_.push_back(p);
        }

    private:
        void fillDistances()
        {
            distance_.resize(point_.size());
            if(distance_.size() == 0)
                return;
            for(agge::count_t i = 1; i < point_.size(); ++i)
                distance_[i] = distance(point_[i-1],point_[i]);
        }

        pod_vector<Point_r> point_;
        pod_vector<real_t>  distance_;
    };

    struct PathFeeder: rules::Consumer<PathFeeder>
    {
        Path& path;

        void move_to(Point_r const& p)
        {
            path.clear();
            path.push_back(p);
        }

        void line_to(Point_r const& p)
        {
            path.push_back(p);
        }
    };

}