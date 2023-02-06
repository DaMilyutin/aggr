#pragma once

#include <plotting/generators/Generator.h>

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
            T start;
            D inc;
            I count;
        public:
            Iota(Iota&&) = default;
            Iota(Iota const&) = default;

            template<typename TT, typename DD, typename II>
            Iota(TT&& t, DD&& d, II& i): start(FWD(t)), inc(FWD(d)), count(FWD(i)) {}

            struct Sentinel {};

            class Iterator
            {
                friend class Iota;
                Iterator(Iota const& i): value(i.start), inc(i.inc), count(i.count) {}
                T value;
                D inc;
                I count;
            public:
                T operator*() const { return value; }
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

    template<typename T, typename I>
    auto linspace(T&& s, T&& e, I&& i)
    {
        static_assert(std::is_integral_v<I>, "type must be integral!");
        return pipeline::Iota<T, pipeline::difference_type<T>, I>{s, (e-s)/(i), i+1};
    }
}