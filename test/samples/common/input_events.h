#pragma once
#include <bitset>
#include <deque>
#include <chrono>
#include <format>
#include <cstdio>

#include <iostream>

namespace system_input
{
    struct Aggregated
    {
        static constexpr size_t MAX_CLICKS = 3;
        bool    pressed = false;
        uint8_t clicks  = 0;

        void click()
        {
            pressed = false;
            if(clicks < MAX_CLICKS)
                ++clicks;
        }

        void reset(bool p = false)
        {
            pressed = p;
            clicks  = 0;
        }

        bool none() const { return !pressed && clicks==0; }
    };

    struct KeyboardKeys
    {
        enum Controls: uint8_t
        {
            Shift, Control, Alt, Win, F10
        };
        std::bitset<256> pressed;
        std::bitset<8>   controls;
    };

    struct MouseButtons
    {
        enum Button
        {
            Left = 0,
            Right,
            Middle,
            Forward,
            Backward,
            Extra_1,
            Extra_2,
            Extra_3,
            Extra_4,
            Extra_5,
            Extra_6,
            Extra_7,
            Extra_8
        };

        struct ButtonState
        {
            Button     button;
            Aggregated state;
        };

        std::bitset<16>  pressed;
        ButtonState      clicked;
    };

    struct Position
    {
        int x = 0;
        int y = 0;
    };

    struct Offset
    {
        int x = 0;
        int y = 0;
    };

    inline Offset operator-(Position const& p1, Position const& p2)
    {
        return {p1.x-p2.x, p1.y-p2.y};
    }

    inline Position operator+(Position const& p, Offset const& o)
    {
        return {p.x+o.x, p.y+o.y};
    }

    inline Position& operator+=(Position& p, Offset const& o)
    {
        p.x += o.x;
        p.y += o.y;
        return p;
    }

    inline Offset& operator+=(Offset& p, Offset const& o)
    {
        p.x += o.x;
        p.y += o.y;
        return p;
    }

    class EventAggregator
    {
    public:
        EventAggregator() {}
        ~EventAggregator() {}
        using Clock = std::chrono::steady_clock;
        using Time     = Clock::time_point;
        using Duration = Time::duration;
        static constexpr std::chrono::milliseconds min_event_threshold{10};
        static constexpr std::chrono::milliseconds click_threshold{500};

        struct Summary
        {
            Position         position;
            Offset           move{};
            int              vWheel = 0;
            int              hWheel = 0;
            MouseButtons     mouse;
            KeyboardKeys     keys;
        };


        void mouse_button(Position p, MouseButtons::Button b, bool down)
        {
            Time time = Clock::now();
            auto const dt = time - mouse_time;
            if(summary.mouse.pressed[b] == down)
                return;
            summary.move = {};
            summary.position = p;
            mouse_time = time;
            summary.mouse.pressed.flip(b);
            if(b != summary.mouse.clicked.button || dt > click_threshold)
            {
                summary.mouse.clicked.state.reset(down);
            }
            else if(!down)
            {
                summary.mouse.clicked.state.click();
                std::puts(std::format("{}: button {} clicked {} times\n",
                            time.time_since_epoch(),
                            int(summary.mouse.clicked.button),
                            int(summary.mouse.clicked.state.clicks)).c_str());
            }

            summary.mouse.clicked.button = b;
        }

        void mouse_wheel(Position p, int vwheel, int hwheel = 0)
        {
            summary.position = p;
            summary.vWheel += vwheel;
            summary.hWheel += hwheel;
        }

        void mouse_move(Position p)
        {
            Offset const d = p - summary.position;
            summary.move = d;
            summary.position = p;
        }

        void key(char k, bool down)
        {
            if(summary.keys.pressed[k] == down)
                return;
            keys_prev = summary.keys.pressed;
            summary.keys.pressed.flip(k);
            //summary.keys.clicked = keys_prev^;
            std::cout << "keys: {:%d}\n" << summary.keys.pressed << std::endl;
        }

        Summary read()
        {
            Summary ret = summary;
            summary.move = {};
            summary.mouse.clicked.state.reset();
            summary.vWheel = 0;
            summary.hWheel = 0;
            return ret;
        }


    private:
        Time             mouse_time;
        Time             keys_time;
        std::bitset<256> keys_prev {};
        Summary          summary   {};
    };
}