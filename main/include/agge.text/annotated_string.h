#pragma once

#include "range_adapter.h"

#include <iterator>
#include <string>
#include <vector>

namespace agge
{
	template <typename CharT, typename AnnotationT>
	class annotated_string
	{
	public:
		typedef std::basic_string<CharT> string_type;
		class range_adapter;
		class const_iterator;

	public:
		annotated_string(const AnnotationT &base_annotation);

		void clear();
		template <typename CharIteratorT>
		void append(CharIteratorT begin, CharIteratorT end);
		void annotate(const AnnotationT &annotation);
		void set_base_annotation(const AnnotationT &annotation);

		bool empty() const;
		size_t size() const;
		const_iterator ranges_begin() const;
		const_iterator ranges_end() const;
		const AnnotationT &current_annotation() const;
		const AnnotationT &base_annotation() const;
		const string_type &underlying() const;

	private:
		typedef std::vector< std::pair<AnnotationT, size_t /*position*/> > annotations_t;

	private:
		string_type _underlying;
		annotations_t _annotations;
		AnnotationT _base_annotation;
	};

	template <typename CharT, typename AnnotationT>
	class annotated_string<CharT, AnnotationT>::range_adapter : public agge::range_adapter<const string_type>
	{
	public:
        range_adapter(const string_type &from);

	public:
		const AnnotationT &get_annotation() const;

	private:
		const AnnotationT *_annotation;

	private:
		friend typename annotated_string<CharT, AnnotationT>::const_iterator;
	};

	template <typename CharT, typename AnnotationT>
	class annotated_string<CharT, AnnotationT>::const_iterator
	{
	public:
		const_iterator(const range_adapter&from, typename annotations_t::const_iterator next_annotation,
			typename annotations_t::const_iterator annotations_end);

		typedef std::forward_iterator_tag iterator_category;
		typedef range_adapter value_type;
		typedef ptrdiff_t difference_type;
		typedef void pointer; // The iterator does not provide stable pointers.
		typedef const range_adapter&reference;

	public:
        range_adapter operator *() const;
		const range_adapter *operator ->() const;
		const_iterator &operator ++();
		bool operator ==(const const_iterator &rhs) const;
		bool operator !=(const const_iterator &rhs) const;

	private:
		void fetch_annotation();

	private:
		range_adapter _current;
		const annotations_t *_annotations;
		typename annotations_t::const_iterator _next_annotation, _annotations_end;
	};



	template <typename CharT, typename AnnotationT>
	inline annotated_string<CharT, AnnotationT>::annotated_string(const AnnotationT &base_annotation)
		: _annotations(1, std::make_pair(base_annotation, 0u)), _base_annotation(base_annotation)
	{	}

	template <typename CharT, typename AnnotationT>
	template <typename CharIteratorT>
	inline void annotated_string<CharT, AnnotationT>::append(CharIteratorT begin, CharIteratorT end)
	{	_underlying.append(begin, end);	}

	template <typename CharT, typename AnnotationT>
	inline void annotated_string<CharT, AnnotationT>::annotate(const AnnotationT &annotation)
	{
		const size_t position = _underlying.size();

		if (_annotations.back().second == position)
			_annotations.back().first = annotation;
		else
			_annotations.push_back(std::make_pair(annotation, position));
	}

	template <typename CharT, typename AnnotationT>
	inline void annotated_string<CharT, AnnotationT>::set_base_annotation(const AnnotationT &annotation)
	{
		_base_annotation = annotation;
		clear();
	}

	template <typename CharT, typename AnnotationT>
	inline void annotated_string<CharT, AnnotationT>::clear()
	{
		_underlying.clear();
		_annotations.assign(1, std::make_pair(_base_annotation, 0u));
	}

	template <typename CharT, typename AnnotationT>
	inline bool annotated_string<CharT, AnnotationT>::empty() const
	{	return _underlying.empty();	}

	template <typename CharT, typename AnnotationT>
	inline size_t annotated_string<CharT, AnnotationT>::size() const
	{	return _underlying.size();	}

	template <typename CharT, typename AnnotationT>
	inline typename annotated_string<CharT, AnnotationT>::const_iterator
		annotated_string<CharT, AnnotationT>::ranges_begin() const
	{	return const_iterator(range_adapter(_underlying), _annotations.begin(), _annotations.end());	}

	template <typename CharT, typename AnnotationT>
	inline typename annotated_string<CharT, AnnotationT>::const_iterator
		annotated_string<CharT, AnnotationT>::ranges_end() const
	{
		range_adapter r(_underlying);

		return r.set_end(), const_iterator(r, _annotations.end(), _annotations.end());
	}

	template <typename CharT, typename AnnotationT>
	inline const AnnotationT &annotated_string<CharT, AnnotationT>::current_annotation() const
	{	return _annotations.back().first;	}

	template <typename CharT, typename AnnotationT>
	inline const AnnotationT &annotated_string<CharT, AnnotationT>::base_annotation() const
	{	return _base_annotation;	}

	template <typename CharT, typename AnnotationT>
	const typename annotated_string<CharT, AnnotationT>::string_type &annotated_string<CharT, AnnotationT>::underlying() const
	{	return _underlying;	}


	template <typename CharT, typename AnnotationT>
	inline annotated_string<CharT, AnnotationT>::range_adapter::range_adapter(const string_type &from)
		: agge::range_adapter<const string_type>(from)
	{	}

	template <typename CharT, typename AnnotationT>
	inline const AnnotationT &annotated_string<CharT, AnnotationT>::range_adapter::get_annotation() const
	{	return *_annotation;	}


	template <typename CharT, typename AnnotationT>
	inline annotated_string<CharT, AnnotationT>::const_iterator::const_iterator(const range_adapter &from,
			typename annotations_t::const_iterator next_annotation,
			typename annotations_t::const_iterator annotations_end)
		: _current(from), _next_annotation(next_annotation), _annotations_end(annotations_end)
	{
		if (_annotations_end != _next_annotation)
			fetch_annotation();
		else
			_current.set_end();
	}

	template <typename CharT, typename AnnotationT>
	inline typename annotated_string<CharT, AnnotationT>::range_adapter
		annotated_string<CharT, AnnotationT>::const_iterator::operator *() const
	{	return _current;	}

	template <typename CharT, typename AnnotationT>
	inline const typename annotated_string<CharT, AnnotationT>::range_adapter
		*annotated_string<CharT, AnnotationT>::const_iterator::operator ->() const
	{	return &_current;	}

	template <typename CharT, typename AnnotationT>
	inline typename annotated_string<CharT, AnnotationT>::const_iterator
		&annotated_string<CharT, AnnotationT>::const_iterator::operator ++()
	{
		_current.begin_index = _current.end_index;
		if (_annotations_end != _next_annotation)
			fetch_annotation();
		return *this;
	}

	template <typename CharT, typename AnnotationT>
	inline bool annotated_string<CharT, AnnotationT>::const_iterator::operator ==(const const_iterator &rhs) const
	{	return _current == rhs._current;	}

	template <typename CharT, typename AnnotationT>
	inline bool annotated_string<CharT, AnnotationT>::const_iterator::operator !=(const const_iterator &rhs) const
	{	return !(*this == rhs);	}

	template <typename CharT, typename AnnotationT>
	inline void annotated_string<CharT, AnnotationT>::const_iterator::fetch_annotation()
	{
		_current._annotation = &_next_annotation++->first;
		if (_annotations_end != _next_annotation)
			_current.end_index = _next_annotation->second;
		else
			_current.extend_end();
	}
}
