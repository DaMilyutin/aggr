#pragma once

#include <agge/utils/minmax.h>
#include <agge/rendering/scanline.h>
#include <agge/utils/tools.h>

namespace agge
{
	class renderer
	{
	public:
		template <typename BitmapT, typename BlenderT>
		class adapter;

	public:
		template <typename BitmapT, typename MaskT, typename BlenderT, typename AlphaFn>
		void operator ()(BitmapT &bitmap_, Vector_i offset, const rect_i *window, const MaskT &mask,
			const BlenderT &blender, const AlphaFn &alpha);

	private:
		raw_memory_object _scanline_cache;
	};


	template <typename BitmapT, typename BlenderT>
	class renderer::adapter
	{
	public:
		typedef typename BlenderT::cover_type cover_type;

	public:
		adapter(BitmapT &bitmap_, Vector_i offset, const rect_i *window, const BlenderT &blender);

		bool set_y(int y);
		void operator ()(int x, int length, const cover_type *covers);

	private:
		static rect_i make_window(BitmapT &bitmap_, Vector_i offset, const rect_i *window);

	private:
		const BlenderT &_blender;
		const Vector_i _offset;
		const rect_i _window;
		typename BitmapT::pixel *_row;
		BitmapT &_bitmap;
		int _y;
	};



	template <typename BitmapT, typename BlenderT>
	inline renderer::adapter<BitmapT, BlenderT>::adapter(BitmapT &bitmap_, Vector_i offset, const rect_i *window,
			const BlenderT &blender)
		: _blender(blender), _offset(offset), _window(make_window(bitmap_, offset, window)), _bitmap(bitmap_)
	{	}

	template <typename BitmapT, typename BlenderT>
	inline bool renderer::adapter<BitmapT, BlenderT>::set_y(int y)
	{
		if (y < _window.min.y || _window.max.y <= y)
			return false;
		_y = y;
		_row = _bitmap.row_ptr(y - _offset.y) - _offset.x;
		return true;
	}

	template <typename BitmapT, typename BlenderT>
	inline void renderer::adapter<BitmapT, BlenderT>::operator ()(int x, int length, const cover_type *covers)
	{
		if (x < _window.min.x)
		{
			const int dx = x - _window.min.x;

			x = _window.min.x;
			length += dx;
			covers -= dx;
		}
		update_min(length, _window.max.x - x);
		if (length > 0)
			_blender(_row + x, x, _y, length, covers);
	}

	template <typename BitmapT, typename BlenderT>
	inline rect_i renderer::adapter<BitmapT, BlenderT>::make_window(BitmapT &bitmap_, Vector_i offset,
		const rect_i * window)
	{
		rect_i w = {
			offset.x, offset.y,
			static_cast<int>(bitmap_.width()) + offset.x, static_cast<int>(bitmap_.height()) + offset.y
		};

		if (window)
		{
			update_max(w.min.x, window->min.x);
			update_max(w.min.y, window->min.y);
			update_min(w.max.x, window->max.x);
			update_min(w.max.y, window->max.y);
		}
		return w;
	}


	template <unsigned _1_shift, typename ScanlineT, typename CellsIteratorT, typename AlphaFn>
	AGGE_INLINE void sweep_scanline(ScanlineT &scanline, CellsIteratorT begin, CellsIteratorT end, const AlphaFn &alpha)
	{
		int cover = 0;

		if (begin == end)
			return;

		for (CellsIteratorT i = begin; ; )
		{
			int x = i->x, area = 0;

			do
			{
				area += i->area;
				cover += i->cover;
				++i;
			} while (i != end && i->x == x);

			int cover_m = cover << (1 + _1_shift);

			if (area)
			{
				scanline.add_cell(x, alpha(cover_m - area));
				++x;
			}

			if (i == end)
				break;

			int len = i->x - x;

			if (len && cover_m)
				scanline.add_span(x, len, alpha(cover_m));
		}
	}


	template <typename ScanlineT, typename MaskT, typename AlphaFn>
	AGGE_INLINE void render(ScanlineT &scanline, const MaskT &mask, const AlphaFn &alpha, int offset, int step)
	{
		for (int y = mask.min_y() + offset, limit_y = mask.min_y() + mask.height(); y < limit_y; y += step)
		{
			typename MaskT::scanline_cells cells = mask[y];

			if (scanline.begin(y))
			{
				sweep_scanline<MaskT::_1_shift>(scanline, cells.first, cells.second, alpha);
				scanline.commit();
			}
		}
	}


	template <typename BitmapT, typename BlenderT>
	inline void fill(BitmapT &bitmap_, const rect_i &area, const BlenderT &blender)
	{
		const int x = agge_max(0, area.min.x);
		const int width = agge_min<int>(bitmap_.width(), area.max.x) - x;

		if (width > 0)
		{
			for (int y = agge_max(0, area.min.y), limit_y = agge_min<int>(bitmap_.height(), area.max.y); y < limit_y; ++y)
				blender(bitmap_.row_ptr(y) + x, x, y, width);
		}
	}


	template <typename BitmapT, typename MaskT, typename BlenderT, typename AlphaFn>
	inline void renderer::operator ()(BitmapT &bitmap_, Vector_i offset, const rect_i *window, const MaskT &mask, const BlenderT &blender,
		const AlphaFn &alpha)
	{
		typedef adapter<BitmapT, BlenderT> rendition_adapter;

		rendition_adapter ra(bitmap_, offset, window, blender);
		scanline_adapter<rendition_adapter> scanline(ra, _scanline_cache, mask.width());

		render(scanline, mask, alpha, 0, 1);
	}
}
