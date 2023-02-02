#pragma once

#include <agge/types.h>
#include <agge/primitives/path.h>

#include <vector>

namespace agge
{
    class polyline
    {
    public:
        friend class polyline_adaptor;

        polyline() = default;
        polyline(polyline&&) = default;
        polyline(polyline const&) = default;

        struct Item
        {
            point_r  point;
            unsigned command;
        };

        using Items = std::vector<Item>;

        void clear() { _items.clear(); }
        void start() { clear(); }

        void move_to(agge::real_t x, agge::real_t y)
        {
            _items.push_back(Item{x, y, agge::path_command_move_to});
        }

        void move_to(agge::point_r p)
        {
            _items.push_back(Item{p, agge::path_command_move_to});
        }

        void line_to(agge::real_t x, agge::real_t y)
        {
            if(!_items.empty())
            {
                if(_items.back().command == agge::path_command_stop)
                    return;
                auto const& prev =_items.back().point;
                float const dist = fabsf(x - prev.x) + fabsf(y - prev.y);
                extern const real_t distance_epsilon;
                if(dist <= distance_epsilon)
                    return;
            }
            _items.push_back(Item{x, y, agge::path_command_line_to});
        }

        void line_to(agge::point_r p)
        {
            if(!_items.empty())
            {
                if(_items.back().command == agge::path_command_stop)
                    return;
                auto const& prev = _items.back().point;
                float const dist = fabsf(p.x - prev.x) + fabsf(p.y - prev.y);
                extern const real_t distance_epsilon;
                if(dist <= distance_epsilon)
                    return;
            }
            _items.push_back(Item{p, agge::path_command_line_to});
        }

        void push_back(Item const& item) { _items.push_back(item); }
        void stop()
        {
            _items.push_back(Item{0.0f, 0.0f, agge::path_command_stop});
        }

    private:
        Items _items;
    };


    class polyline_adaptor
    {
    public:
        polyline_adaptor(polyline const& path)
            : _path(path), _position(_path._items.begin())
        {}

        void rewind(unsigned i)
        {
            _position = _path._items.begin() + i;
        }

        unsigned vertex(agge::real_t* x, agge::real_t* y)
        {
            if(_position == _path._items.end())
                return agge::path_command_stop;
            else
                return *x = _position->point.x, *y = _position->point.y, _position++->command;
        }

    private:
        const polyline& _path;
        polyline::Items::const_iterator _position;
    };

}
