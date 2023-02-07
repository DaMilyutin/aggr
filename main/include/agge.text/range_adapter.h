#pragma once

#include <cstddef>

namespace agge
{
	template <typename ContainerT>
	class range_adapter
	{
	public:
		typedef typename ContainerT::const_iterator const_iterator;

	public:
		range_adapter(ContainerT &container);

		void set_end();
		void extend_end();
		bool empty() const;
		typename ContainerT::const_iterator begin() const;
		typename ContainerT::const_iterator end() const;

	public:
		size_t begin_index, end_index;

	private:
		ContainerT *_container;
	};



	template <typename ContainerT>
	inline range_adapter<ContainerT>::range_adapter(ContainerT &container)
		: begin_index(0u), end_index(0u), _container(&container)
	{	}

	template <typename ContainerT>
	inline void range_adapter<ContainerT>::set_end()
	{	extend_end(), begin_index = end_index;	}

	template <typename ContainerT>
	inline void range_adapter<ContainerT>::extend_end()
	{	end_index = _container->size();	}

	template <typename ContainerT>
	inline bool range_adapter<ContainerT>::empty() const
	{	return begin_index == end_index;	}

	template <typename ContainerT>
	inline typename ContainerT::const_iterator range_adapter<ContainerT>::begin() const
	{	return _container->begin() + begin_index;	}

	template <typename ContainerT>
	inline typename ContainerT::const_iterator range_adapter<ContainerT>::end() const
	{	return _container->begin() + end_index;	}


	template <typename ContainerT>
	inline bool operator ==(const range_adapter<ContainerT> &lhs, const range_adapter<ContainerT> &rhs)
	{	return lhs.begin_index == rhs.begin_index && lhs.end_index == rhs.end_index;	}
}
