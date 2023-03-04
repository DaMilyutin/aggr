#pragma once
#include <palgebra/algebra/rules.h>
#include <agge/utils/vertex_sequence.h>
#include <agge/utils/pod_sequence.h>



namespace agge
{
    struct Vertex
    {
        Point_r point;
        real_t  distance = 0.0f;
    };

    typedef pod_vector<Vertex> Path;

    struct PathFeeder: rules::Rasterizer<PathFeeder>
    {
        Path& path;

        void move_to(Point_r const& p)
        {
            path.clear();
            path.push_back(Vertex{p, 0.0f});
        }

        void line_to(Point_r const& p)
        {
            if(path.empty())
                return move_to(p);
            auto const& l = path.back();
            path.push_back(Vertex{p, distance(p, l.point) + l.distance});
        }
    }

}