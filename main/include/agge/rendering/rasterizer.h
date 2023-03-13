#pragma once

#include <agge/math/math.h>
#include <grace/algebra/rules.h>
#include <grace/algebra/control.h>
#include <grace/types/Point.h>
#include "vector_rasterizer.h"

namespace agge
{
	template <typename T>
	struct scaling;

    namespace rules
    {
        template<typename E>
        struct Start;

        template<> struct Start<Point_r>        { Point_r under; };
        template<> struct Start<Point_r const&> { Point_r const& under; };
    }


	template < typename ClipperT, typename ScalingT = scaling<typename ClipperT::coord_type> >
	class rasterizer: private vector_rasterizer
	{
	public:
		using vector_rasterizer::_1_shift;
		using vector_rasterizer::scanline_cells;

	public:
		using vector_rasterizer::reset;

		void reset_clipping();
		void set_clipping(const Rect<real_t> &window);

		void move_to(real_t x, real_t y);
		void line_to(real_t x, real_t y);


        void move_to(Point_r const& p) { move_to(p.x, p.y); }
        void line_to(Point_r const& p) { line_to(p.x, p.y); }

		void close_polygon();

		void append(const rasterizer &other, int dx, int dy);

		using vector_rasterizer::sort;
		using vector_rasterizer::compact;
		using vector_rasterizer::operator [];
		using vector_rasterizer::width;
		using vector_rasterizer::min_y;
		using vector_rasterizer::height;

	private:
		typedef typename ClipperT::coord_type coord_type;

	private:
		void line(coord_type x1, coord_type y1, coord_type x2, coord_type y2);

	private:
		ClipperT _clipper;
		coord_type _start_x, _start_y;

	private:
		friend ClipperT;
	};

    template<typename R>
    class Rasterizer: public grace::rules::Sink<Rasterizer<R>>
    {
    public:
        Rasterizer(R& u): under(u) {}

        void reset() { move_ = true; under.reset(); }

        // Sink
        Rasterizer& operator<<(Point_r const& p) { consume(p); return *this; }
        Rasterizer& operator<<(grace::rules::start_token) { move_ = true; return *this; }
        Rasterizer& operator<<(grace::rules::close_token) { under.close_polygon(); return *this; }

        bool consume(grace::Point_r const& p)
        {
            if(move_) move_to(p);
            else      line_to(p);
            return true;
        }

        void move_to(grace::Point_r const& p) { under.move_to(p.x, p.y); move_ = false; }
        void line_to(grace::Point_r const& p) { under.line_to(p.x, p.y); }

        void close_polygon() { under.close_polygon(); }
    private:
        R&   under;
        bool move_ = true;
    };

    template<typename R>
    Rasterizer<R> wrap_rasterizer(R& ras) { return {ras}; }



	template <>
	struct scaling<int>
	{
		static void scale1(real_t x, real_t y, int &cx, int &cy)
		{	cx = iround(256.0f * x), cy = iround(256.0f * y);	}

		static void scale2(int x1, int y1, int x2, int y2, int &cx1, int &cy1, int &cx2, int &cy2)
		{	cx1 = x1, cy1 = y1, cx2 = x2, cy2 = y2;	}
	};

	template <>
	struct scaling<real_t>
	{
		static void scale1(real_t x, real_t y, real_t &cx, real_t &cy)
		{	cx = x, cy = y;	}

		static void scale2(real_t x1, real_t y1, real_t x2, real_t y2, int &cx1, int &cy1, int &cx2, int &cy2)
		{
			cx1 = iround(256.0f * x1);
			cy1 = iround(256.0f * y1);
			cx2 = iround(256.0f * x2);
			cy2 = iround(256.0f * y2);
		}
	};


	template <typename ClipperT, typename ScalingT>
	inline void rasterizer<ClipperT, ScalingT>::reset_clipping()
	{	_clipper.reset();	}

	template <typename ClipperT, typename ScalingT>
	inline void rasterizer<ClipperT, ScalingT>::set_clipping(const Rect<real_t> &window)
	{
		Rect<typename ClipperT::coord_type> translated;

		ScalingT::scale1(window.min.x, window.min.y, translated.min.x, translated.min.y);
		ScalingT::scale1(window.max.x, window.max.y, translated.max.x, translated.max.y);
		_clipper.set(translated);
	}

	template <typename ClipperT, typename ScalingT>
	inline void rasterizer<ClipperT, ScalingT>::move_to(real_t x, real_t y)
	{
		ScalingT::scale1(x, y, _start_x, _start_y);
		_clipper.move_to(_start_x, _start_y);
	}

	template <typename ClipperT, typename ScalingT>
	inline void rasterizer<ClipperT, ScalingT>::line_to(real_t x, real_t y)
	{
		coord_type cx, cy;

		ScalingT::scale1(x, y, cx, cy);
		_clipper.line_to(*this, cx, cy);
	}

	template <typename ClipperT, typename ScalingT>
	inline void rasterizer<ClipperT, ScalingT>::close_polygon()
	{	_clipper.line_to(*this, _start_x, _start_y);	}

	template <typename ClipperT, typename ScalingT>
	inline void rasterizer<ClipperT, ScalingT>::append(const rasterizer &other, int dx, int dy)
	{
		int cx, cy, unused;

		ScalingT::scale2(dx, dy, 0, 0, cx, cy, unused, unused);
		vector_rasterizer::append(other, cx, cy);
	}

	template <typename ClipperT, typename ScalingT>
	inline void rasterizer<ClipperT, ScalingT>::line(coord_type x1, coord_type y1, coord_type x2, coord_type y2)
	{
		int cx1, cy1, cx2, cy2;

		ScalingT::scale2(x1, y1, x2, y2, cx1, cy1, cx2, cy2);
		vector_rasterizer::line(cx1, cy1, cx2, cy2);
	}
}
