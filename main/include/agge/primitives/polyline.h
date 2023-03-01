#pragma once

#include <agge/types.h>
#include <agge/primitives/path.h>
#include <agge/primitives/pipeline.h>

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
            Point_r  point;
            unsigned command;
        };

        using Items = std::vector<Item>;

        polyline& clear() { _items.clear(); return *this; }
        polyline& start() { _items.clear(); return *this; }

        polyline& move_to(agge::real_t x, agge::real_t y)
        {
            _items.push_back(Item{x, y, agge::path_command_move_to});
            return *this;
        }

        polyline& move_to(agge::Point_r p)
        {
            _items.push_back(Item{p, agge::path_command_move_to});
            return *this;
        }

        polyline& line_to(agge::real_t x, agge::real_t y)
        {
            return line_to({x, y});
        }

        polyline& line_to(agge::Point_r p)
        {
            if(_items.empty())
                _items.push_back(Item{p, agge::path_command_move_to});
            if(_items.back().command == agge::path_command_stop
                || too_small_step(p))
                return *this;
            _items.push_back(Item{p, agge::path_command_line_to});
            return *this;
        }

        polyline& operator<<(agge::Point_r p) { return line_to(p); }
        polyline& operator<<(Item i) { _items.push_back(i); return *this; }

        void push_back(Item item) { _items.push_back(item); }
        void stop()
        {
            _items.push_back(Item{0.0f, 0.0f, agge::path_command_stop});
        }

        Items const& items() const { return _items; }
    private:
        bool too_small_step(Point_r const& p) const
        {
            auto const& prev = _items.back().point;
            float const dist = fabsf(p.x - prev.x) + fabsf(p.y - prev.y);
            extern const real_t distance_epsilon;
            return dist <= distance_epsilon;
        }

        Items _items;
    };

    inline polyline& clear(polyline& p) { return p.clear(); }
    inline polyline& operator<<(polyline& p, polyline& (*f)(polyline&)) { return f(p); }

    class polyline_adaptor: public pipeline::terminal<polyline_adaptor>
    {
    public:
        polyline_adaptor(polyline const& path)
            : _path(path), _position(_path._items.begin())
        {}

        void rewind(unsigned i = 0u)
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
