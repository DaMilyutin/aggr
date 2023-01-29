#include <agge/dash.h>

#include <agge/path.h>

namespace agge
{
	dash::dash()
		: _dash_start(0.0f), _state(initial)
	{	}

	void dash::remove_all_dashes()
	{	_pattern.clear(); }

	void dash::add_dash(real_t dash_length, real_t gap_length)
	{
		dash_gap dg = { dash_length, gap_length };

		_pattern.push_back(dg);
	}

	void dash::dash_start(real_t offset)
	{	_dash_start = offset;	}

	void dash::remove_all()
	{
		vertex_sequence::clear();
		_state = initial;
	}

	void dash::close_polygon()
	{	}

	void dash::add_vertex(real_t x, real_t y, int command)
	{
		if (empty())
			_start.x = x, _start.y = y;
		else if (command & path_flag_close)
			add_polyline_vertex(*this, _start.x, _start.y, path_command_line_to);
		add_polyline_vertex(*this, x, y, command);
	}

	int dash::vertex(real_t *x, real_t *y)
	{
		switch (_state)
		{
		case initial:
			_j = begin();
			seek_dash_start();

		case seek:
			do
			{
				_t -= _j->distance;
				if (++_j == end())
				{
					_state = complete;
					return path_command_stop;
				}
			}	while (_t > 0.0f);

		case move:
			interpolate_current(x, y);
			_t += _dash_length;
			_state = _t > 0.0f ? emit_source : finish_dash;
			return path_command_move_to;

		case emit_source:
			*x = _j->point.x, *y = _j->point.y;
			_t -= _j->distance;
			if (++_j == end())
				_state = complete;
			else if (_t < 0.0f)
				_state = finish_dash;
			return path_command_line_to;

		case finish_dash:
			interpolate_current(x, y);
			_t += _dash->gap_length;
			if (++_dash == _pattern.end())
				_dash = _pattern.begin();
			_dash_length = _dash->dash_length;
			_state = _t < 0.0f ? move : seek;
			return path_command_line_to;

		case complete:
			return path_command_stop;
		}
		return path_command_stop;
	}

	void dash::seek_dash_start()
	{
		real_t period = 0.0f;

		for (agge::pod_vector<dash_gap>::const_iterator i = _pattern.begin(); i != _pattern.end(); ++i)
			period += i->dash_length + i->gap_length;

		real_t dash_start = _dash_start - period * static_cast<int>(_dash_start / period);

		for (_dash = _pattern.begin(); ; _dash++)
		{
			const real_t d = _dash->dash_length + _dash->gap_length;

			if (dash_start < d)
				break;
			dash_start -= d;
		}

		const real_t dash_length = _dash->dash_length - dash_start;

		if (dash_length > 0.0f)
			_t = 0.0f, _dash_length = dash_length; // dash is hit
		else
			_t = _dash->gap_length + dash_length, _dash_length = _dash->dash_length; // gap is hit
	}

	void dash::interpolate_current(real_t *x, real_t *y) const
	{
		vertex_sequence::const_iterator i = _j - 1;
		point_r m = _j->point + (_t / i->distance) * (_j->point - i->point);

		*x = m.x, *y = m.y;
	}
}
