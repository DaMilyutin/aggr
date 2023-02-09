#pragma once
#include <plotting/generators/abstract.h>

namespace plotting
{
    namespace pipeline
    {
        template<typename Y, typename L>
        class YieldLink: Yield<YieldLink<Y, L>>
        {
        public:
            template<typename TY, typename TL>
            YieldLink(TY&& y, TL&& l): yield(FWD(y)), link(FWD(l)) {}

            // this is base helper class
            // all functions to be in derived

            Y yield;
            L link;
        };

        template<typename L1, typename L2>
        class LinkLink: Link<LinkLink<L1, L2>>
        {
        public:
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

        template<typename L, typename S>
        class LinkSink: Sink<LinkSink<L, S>>
        {
        public:
            template<typename TL, typename TS>
            LinkSink(TL&& l, TS&& s): link(FWD(l)), sink(FWD(s)) {}

            template<typename E>
            bool operator()(E&& e) { return link.feed(sink, e); }

            L link;
            S sink;
        };

        template<typename Y, typename L>
        YieldLink<Y, L> perfection(Yield<Y>&& y, Link<L>&& l)
        {
            return {FWD(y)._get_(), FWD(l)._get_()};
        }

        template<typename L1, typename L2>
        LinkLink<L1, L2> perfection(Link<L1>&& l1, Link<L2>&& l2)
        {
            return {FWD(l1)._get_(), FWD(l2)._get_()};
        }

        template<typename L, typename S>
        LinkSink<L, S> perfection(Link<L>&& l, Sink<S>&& s)
        {
            return {FWD(l)._get_(), FWD(s)._get_()};
        }

        template<typename L1, typename L2, typename S>
        LinkSink<L1, LinkSink<L2, S>> perfection(LinkLink<L1, L2>&& ll, Sink<S>&& s)
        {
            return LinkSink<L1, LinkSink<L2, S>>{FWD(ll).link1, LinkSink<L2, S>(FWD(ll).link2, FWD(s)._get_())}
        }

        template<typename Y, typename L, typename S>
        bool perfection(YieldLink<Y, L>&& yl, Sink<S>&& s)
        {
            return perfection(yl.yield, LinkSink<L, S>(yl.link, FWD(s)._get_()));
        }

        template<typename Y, typename S>
        bool perfection(Yield<Y>&& yield, Sink<S>&& sink)
        {
            for(auto&& e: FWD(yield)._get_())
               if(!sink(e))
                    return false; // if sink forced to stop
            return true;
        }
    }
}