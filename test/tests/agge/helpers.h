#pragma once

#include <agge/utils/math.h>
#include <agge/primitives/path.h>
#include <agge/pixel.h>
#include <agge/utils/tools.h>
#include <agge/types.h>

#include <algorithm>
#include <vector>

namespace agge
{
	template <typename T>
	class pod_vector;

	namespace tests
	{
		template <typename T, size_t n, int alignment_order = 4>
		class aligned_array
		{
		public:
			typedef T value_type;

		public:
			aligned_array();

			const T *begin() const;
			const T *end() const;

			T *data();
			const T *data() const;
			T &operator [](size_t index);

			void fill(const T &value);

		private:
			union pod_constraint { T unused1; int unused2; };
			enum { alignment = 1 << alignment_order, alignment_mask = alignment_order ? alignment - 1 : 0 };

		private:
			aligned_array(const aligned_array &other);
			const aligned_array &operator =(const aligned_array &rhs);

			size_t aligned_offset() const;

		private:
			uint8_t _buffer[sizeof(T) * n + alignment];
		};



		template <typename T, size_t n, int alignment_order>
		inline aligned_array<T, n, alignment_order>::aligned_array()
		{	}

		template <typename T, size_t n, int alignment_order>
		inline const T *aligned_array<T, n, alignment_order>::begin() const
		{	return data();	}

		template <typename T, size_t n, int alignment_order>
		inline const T *aligned_array<T, n, alignment_order>::end() const
		{	return data() + n;	}

		template <typename T, size_t n, int alignment_order>
		inline T *aligned_array<T, n, alignment_order>::data()
		{	return reinterpret_cast<T *>(&_buffer[aligned_offset()]);	}

		template <typename T, size_t n, int alignment_order>
		inline const T *aligned_array<T, n, alignment_order>::data() const
		{	return reinterpret_cast<const T *>(&_buffer[aligned_offset()]);	}

		template <typename T, size_t n, int alignment_order>
		inline T &aligned_array<T, n, alignment_order>::operator [](size_t index)
		{	return *(data() + index);	}

		template <typename T, size_t n, int alignment_order>
		inline void aligned_array<T, n, alignment_order>::fill(const T &value)
		{	std::fill_n(data(), n, value);	}

		template <typename T, size_t n, int alignment_order>
		inline size_t aligned_array<T, n, alignment_order>::aligned_offset() const
		{	return (alignment - (reinterpret_cast<size_t>(_buffer) & alignment_mask)) & alignment_mask;	}

		template <typename T, size_t n, int alignment_order>
		inline bool operator ==(const std::vector<T> &lhs, const aligned_array<T, n, alignment_order> &rhs)
		{	return lhs.size() == n && std::equal(lhs.begin(), lhs.end(), rhs.data());	}


		inline pixel32 make_pixel(uint8_t c0, uint8_t c1, uint8_t c2, uint8_t c3)
		{
			pixel32 p = { { c0, c1, c2, c3 } };
			return p;
		}

		template <typename T, size_t N>
		inline std::vector<T> mkvector(T (&p)[N])
		{	return std::vector<T>(p, p + N);	}

		template <typename T>
		inline agge_vector<T> mkvector(T dx, T dy)
		{
			agge_vector<T> v = { dx, dy };
			return v;
		}

		template <typename T>
		inline bool is_on_segment(const point<T>& t, const point<T>& segment_a, const point<T>& segment_b)
		{
			T xp = (t.x - segment_a.x) * (segment_a.y - segment_b.y) - (t.y - segment_a.y) * (segment_a.x - segment_b.x);

			if (!equal(xp, T()))
				return false;
			return agge_min(segment_a.x, segment_b.x) <= t.x && t.x <= agge_min(segment_a.x, segment_b.x)
				&& agge_min(segment_a.y, segment_b.y) <= t.y && t.y <= agge_min(segment_a.y, segment_b.y);
		}

		template <typename T>
		inline void move_to(T &acceptor, real_t x, real_t y)
		{	acceptor.add_vertex(x, y, path_command_move_to);	}

		template <typename T>
		inline void line_to(T &acceptor, real_t x, real_t y, bool close = false)
		{	acceptor.add_vertex(x, y, path_command_line_to | (close ? path_flag_close : 0));	}

		template <typename T>
		inline void end_poly(T &acceptor, bool close)
		{	acceptor.add_vertex(0.0f, 0.0f, path_command_end_poly | (close ? path_flag_close : 0));	}
	}
}
