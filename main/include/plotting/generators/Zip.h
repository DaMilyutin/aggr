#pragma once
#include <plotting/generators/Generator.h>
#include <tuple>
#include "aFWD.h"

namespace plotting
{
    namespace pipeline
    {
        template <typename... T>
        class ZipGenerator: public Generator<ZipGenerator<T...>>
        {
        public:
            class Iterator;
            class Sentinel
            {
                friend class Iterator;
                std::tuple<decltype(std::declval<T>().end())...> sentinels_;
            public:
                explicit Sentinel(decltype(sentinels_) s): sentinels_{std::move(s)} {}
            };

            class Iterator
            {
            public:
                using value_type = std::tuple<decltype(*std::declval<T>().begin())...>;
            private:
                std::tuple<decltype(std::declval<T>().begin())...> iters_;

                template <std::size_t... I>
                auto deref(std::index_sequence<I...>) const
                {
                    return typename Iterator::value_type{*std::get<I>(iters_)...};
                }

                template <std::size_t... I>
                void increment(std::index_sequence<I...>)
                {
                    auto l = {(++std::get<I>(iters_), 0)...};
                }

            public:
                explicit Iterator(decltype(iters_) iters): iters_{std::move(iters)} {}

                Iterator& operator++()
                {
                    increment(std::index_sequence_for<T...>{});
                    return *this;
                }

                Iterator operator++(int)
                {
                    auto saved{*this};
                    increment(std::index_sequence_for<T...>{});
                    return saved;
                }

                bool operator!=(const Sentinel& other) const
                {
                    return iters_ != other.sentinels_;
                }

                auto operator*() const { return deref(std::index_sequence_for<T...>{}); }
            };

            ZipGenerator(T&&... seqs)
                : _seqs(std::forward<T>(seqs))
            {}

            Iterator begin() const { return std::make_tuple(_seqs.begin()...); }
            Sentinel end() const { return std::make_tuple(_seqs.end()...); }

        private:
            std::tuple<T...> _seqs;
        };


        template <typename... T>
        class ZipGenerator<T const&...>
        {
        public:
            class Sentinel
            {
                std::tuple<decltype(std::declval<T>().end())...> sentinels_;
            public:
                explicit Sentinel(decltype(sentinels_) s): sentinels_{std::move(s)} {}
            };

            class Iterator
            {
            public:
                using value_type = std::tuple<decltype(remove_ref(*std::declval<T const>().begin()))...>;
                //using value_type = std::tuple<decltype(*std::declval<T const>().begin())...>;
            private:
                std::tuple<decltype(std::declval<T const>().begin())...> iters_;

                template <std::size_t... I>
                auto deref(std::index_sequence<I...>) const
                {
                    return typename Iterator::value_type{*std::get<I>(iters_)...};
                }

                template <std::size_t... I>
                void increment(std::index_sequence<I...>)
                {
                    auto l = {(++std::get<I>(iters_), 0)...};
                }

            public:
                explicit Iterator(decltype(iters_) iters): iters_{std::move(iters)} {}

                Iterator& operator++()
                {
                    increment(std::index_sequence_for<T...>{});
                    return *this;
                }

                Iterator operator++(int)
                {
                    auto saved{*this};
                    increment(std::index_sequence_for<T...>{});
                    return saved;
                }

                bool operator!=(const Sentinel& other) const
                {
                    return iters_ != other.sentinels_;
                }

                auto operator*() const { return deref(std::index_sequence_for<T...>{}); }
            };

            ZipGenerator(T const&... seqs)
                : begin_{std::make_tuple(seqs.begin()...)}
                , end_{std::make_tuple(seqs.end()...)}
            {
            }

            Iterator begin() const { return begin_; }
            Sentinel end() const { return end_; }

        private:
            Iterator begin_;
            Sentinel end_;
        };

        // Sequences must be the same length.
        template <typename... T>
        auto zip(Generator<T>&&... seqs)
        {
            return ZipGenerator<T...>{FWD(seqs._get_())};
        }

        // Sequences must be the same length.
        template <typename... T>
        auto zip(Generator<T> const&... seqs)
        {
            return ZipGenerator<T const&...>{FWD(seqs._get_())};
        }

     }

     using pipeline::zip;
}