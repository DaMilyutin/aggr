#pragma once

#include <agge/types.h>

namespace agge
{
	template <typename T>
	void memset(T *buffer, T value, count_t count);

	class raw_memory_object : noncopyable
	{
	public:
		raw_memory_object() throw();
		~raw_memory_object() throw();

		template <typename T>
		T *get(count_t size);

	private:
		template <typename T>
		T *get_adjust(count_t size);

	private:
		uint8_t *_buffer;
		count_t _size;
	};



	inline raw_memory_object::raw_memory_object() throw()
		: _buffer(0), _size(0)
	{	}

	inline raw_memory_object::~raw_memory_object() throw()
	{	delete []_buffer;	}

	template <typename T>
	inline T *raw_memory_object::get(count_t size)
	{
		size *= sizeof(T);
		size /= sizeof(uint8_t);
		return size > _size ? get_adjust<T>(size) : reinterpret_cast<T *>(_buffer);
	}

	template <typename T>
	inline T *raw_memory_object::get_adjust(count_t size)
	{
		uint8_t *buffer = new uint8_t[size];

		delete []_buffer;
		_buffer = buffer;
		_size = size;
		memset(buffer, uint8_t(), size);
		return reinterpret_cast<T *>(_buffer);
	}


	template <typename T>
	inline void memset(T *buffer, T value, count_t count)
	{
		if (0 == (count & ~0x03))
		{
			if (count--)
				*buffer++ = value;
			if (count--)
				*buffer++ = value;
			if (count--)
				*buffer++ = value;
		}
		else
		{
			while (count--)
				*buffer++ = value;
		}
	}
}
