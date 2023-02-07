#pragma once

#include <plotting/generators/generator.h>

namespace plotting
{
    namespace pipeline
    {
        template<typename T>
        using difference_type = std::remove_all_extents_t<decltype(std::declval<T>()-std::declval<T>())>;

        struct Infinite
        {
            constexpr Infinite() = default;
            constexpr Infinite(auto) {}
            constexpr void operator--() {}
            constexpr bool operator!() const { return false; }
        };

        template<typename T, typename D = difference_type<T>, typename I = size_t>
        class Iota: public Generator<Iota<T>>
        {
            T _start;
            D _inc;
            I _count;
        public:
            Iota(Iota&&) = default;
            Iota(Iota const&) = default;

            template<typename TT, typename DD, typename II>
            Iota(TT&& t, DD&& d, II&& i): _start(FWD(t)), _inc(FWD(d)), _count(FWD(i)) {}

            struct Sentinel {};

            I count() const { return _count; }

            class Iterator
            {
                friend class Iota;
                Iterator(Iota const& i): value(i._start), inc(i._inc), count(i._count) {}
                T value;
                D inc;
                I count;
            public:
                T const& operator*() const { return value; }
                Iterator& operator++() { --count; value += inc; return *this; }
                bool operator==(Sentinel) const { return !count; }
                bool operator!=(Sentinel) const { return !!count; }
            };

            Iterator begin() const { return Iterator(*this); }
            Sentinel end() const { return Sentinel{}; }
        };
    }

    template<typename T, typename D, typename I>
    auto iota(T t, D d, I i)
    {
        return pipeline::Iota<T, D, I>{t, d, i};
    }

    template<typename T, typename D>
    auto iota(T t, D d)
    {
        return pipeline::Iota<T, D, pipeline::Infinite>(t, d, pipeline::Infinite());
    }

    template<typename T, typename I>
    auto linspace(T b, T e, I i)
    {
        static_assert(std::is_integral_v<I>, "type must be integral!");
        return pipeline::Iota<T, pipeline::difference_type<T>, I>{b, (e-b)/(i), i+1};
    }

    template<typename T>
    auto linrange(T b, T e, T step)
    {
        return pipeline::Iota<T, pipeline::difference_type<T>, size_t>{b, step, size_t((e-b)/(step))};
    }
}