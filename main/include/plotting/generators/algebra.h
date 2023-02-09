#pragma once
#include <plotting/generators/abstract.h>

namespace plotting
{
    namespace pipeline
    {
        template<typename Y, typename L, typename tag = void>
        struct YieldLink: Yield<YieldLink<Y, L, tag>, tag>
        {
            template<typename TY, typename TL>
            YieldLink(TY&& y, TL&& l): yield(FWD(y)), link(FWD(l)) {}

            auto begin() const { return link.begin(yield); }
            auto end()   const { return link.end(yield); }

            Y yield;
            L link;
        };

        template<typename L, typename S, typename tag = void>
        struct LinkSink: Sink<LinkSink<L, S, tag>, tag>
        {
            template<typename TL, typename TS>
            LinkSink(TL&& l, TS&& s): link(FWD(l)), sink(FWD(s)) {}

            template<typename E>
            bool operator()(E&& e) { return link.feed(sink, e); }

            L link;
            S sink;
        };

        template<typename L1, typename L2, typename tag = void>
        struct LinkLink: Link<LinkLink<L1, L2, tag>, tag>
        {
            template<typename T1, typename T2>
            LinkLink(T1&& l1, T2&& l2): link1(FWD(l1)), link2(FWD(l2)) {}

            template<typename S, typename E>
            bool feed(S& sink, E&& e)
            {
                LinkSink<L2&, S&> sink2(link2, sink);
                return link1.feed(sink2, FWD(e));
            }

            L1 link1;
            L2 link2;
        };

        // binary operations
        template<typename Y, typename L, typename tag>
        YieldLink<Y, L, tag> perfection(Yield<Y, tag>&& y, Link<L, tag>&& l)
        {
            return {FWD(y)._get_(), FWD(l)._get_()};
        }

        template<typename Y, typename L, typename tag>
        YieldLink<Y const&, L, tag> perfection(Yield<Y, tag> const& y, Link<L, tag>&& l)
        {
            return {y._get_(), FWD(l)._get_()};
        }

        template<typename Y, typename L, typename tag>
        YieldLink<Y, L const&, tag> perfection(Yield<Y, tag>&& y, Link<L, tag> const& l)
        {
            return {FWD(y)._get_(), l._get_()};
        }

        template<typename Y, typename L, typename tag>
        YieldLink<Y const&, L const&, tag> perfection(Yield<Y, tag> const& y, Link<L, tag> const& l)
        {
            return {y._get_(), l._get_()};
        }

        template<typename L1, typename L2, typename tag>
        LinkLink<L1, L2, tag> perfection(Link<L1, tag>&& l1, Link<L2, tag>&& l2)
        {
            return {FWD(l1)._get_(), FWD(l2)._get_()};
        }

        template<typename L, typename S, typename tag>
        LinkSink<L, S, tag> perfection(Link<L, tag>&& l, Sink<S, tag>&& s)
        {
            return {FWD(l)._get_(), FWD(s)._get_()};
        }

        // Yield + Sink => system closed and ready to run
        template<typename Y, typename S, typename tag>
        bool perfection(Yield<Y, tag>&& yield, Sink<S, tag>&& sink)
        {
            for(auto&& e: FWD(yield)._get_())
                if(!sink(e))
                    return false; // if sink forced to stop
            return true;
        }

        // Yield + Sink: We prefer to keep Yield simple
        template<typename Y, typename L, typename S, typename tag>
        bool perfection(YieldLink<Y, L, tag>&& yl, Sink<S, tag>&& sink)
        {
            return perfection(FWD(yl).yield, perfection(FWD(yl).link, FWD(sink)));
        }

        // reorganize pipeline around sink
        template<typename L1, typename L2, typename S, typename tag>
        LinkSink<L1, LinkSink<L2, S, tag>, tag> perfection(LinkLink<L1, L2, tag>&& ll, Sink<S, tag>&& s)
        {
            return perfection(FWD(ll).link1, perfection(FWD(ll).link2, FWD(s)));
        }

        // reorganize pipeline around yield
        template<typename Y, typename L1, typename L2, typename tag>
        YieldLink<YieldLink<Y, L1, tag>, L2, tag> perfection(Yield<Y, tag>&& y, LinkLink<L1, L2, tag>&& ll)
        {
            return perfection(perfection(FWD(y), FWD(ll).link1), FWD(ll).link2);
        }

        template<typename Y, typename L>
        auto operator/(Yield<Y>&& y, Link<L>&& l)
        {
            return perfection(FWD(y), FWD(l));
        }

        template<typename Y, typename L>
        auto operator/(Yield<Y> const& y, Link<L> const& l)
        {
            return perfection(FWD(y), FWD(l));
        }

        template<typename Y, typename L>
        auto operator/(Yield<Y> const& y, Link<L>&& l)
        {
            return perfection(y, FWD(l));
        }

        template<typename Y, typename L>
        auto operator/(Yield<Y>&& y, Link<L> const& l)
        {
            return perfection(FWD(y), l);
        }

    }
}