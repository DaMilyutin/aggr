#include <agge/primitives/curves.h>

#include <agge/math/math.h>
#include <agge/primitives/path.h>

namespace agge
{
	//qbezier::qbezier(Point_r b, Point_r c, Point_r e, real_t step)
	//	: qbezier_interp(b, c, e), _t(0.0f), _step(step), _stage(path_command_move_to)
	//{	}

	//void qbezier::rewind(unsigned /*id*/)
	//{
	//	_t = 0.0f;
	//	_stage = path_command_move_to;
	//}

 //   int qbezier::vertex(Point_r* p)
 //   {
 //       int stage = _stage;

 //       switch(stage)
 //       {
 //       case path_command_move_to:
 //           *p = _b;
 //           _stage = path_command_line_to;
 //           _t = _step;
 //           break;

 //       case path_command_line_to:
 //           if(_t < 1.0f)
 //           {
 //               *p = at(_t);
 //               _t += _step;
 //           }
 //           else
 //           {
 //               *p = _e;
 //               _stage = path_command_stop;
 //           }
 //           break;
 //       }
 //       return stage;
 //   }

	//cbezier::cbezier(Point_r b, Point_r c1, Point_r c2, Point_r e, real_t step)
	//	: cbezier_interp(b, c1, c2, e), _step(step)
 //       , _stage(path_command_move_to)
	//{}

	//void cbezier::rewind(unsigned /*id*/)
	//{
	//	_t = 0.0f;
	//	_stage = path_command_move_to;
	//}

 //   int cbezier::vertex(Point_r* p)
 //   {
 //       int stage = _stage;

 //       switch(stage)
 //       {
 //       case path_command_move_to:
 //           *p = _b;
 //           _stage = path_command_line_to;
 //           _t = _step;
 //           break;

 //       case path_command_line_to:
 //           if(_t < 1.0f)
 //           {
 //               *p = at(_t);
 //               _t += _step;
 //           }
 //           else
 //           {
 //               *p = _e;
 //               _stage = path_command_stop;
 //           }
 //           break;
 //       }
 //       return stage;
 //   }






	arc::arc(real_t cx, real_t cy, real_t r, real_t start, real_t end, real_t /*da*/)
		: _cx(cx), _cy(cy), _r(r), _start(start), _end(end), _step(end > start ? 0.05f : -0.05f)
	{	rewind(0);	}

	void arc::rewind(unsigned /*id*/)
	{
		_a = _start;
		_stage = path_command_move_to;
	}

	int arc::vertex(real_t *x, real_t *y)
	{
		int stage = _stage;

		if (stage == path_command_move_to)
			_stage = path_command_line_to;
		else if (stage == path_command_stop)
			return stage;
		if ((_a < _end) ^ (_end > _start))
		{
			_a = _end;
			_stage = path_command_stop;
		}
		*x = _cx + _r * cos(_a);
		*y = _cy + _r * sin(_a);
		_a += _step;
		return stage;
	}


    Interval::Interval(real_t step)
        : _t(0.0f), _step(step), _stage(path_command_move_to)
    {}

    int Interval::vertex(real_t* t)
    {
        int stage = _stage;
        switch(stage)
        {
        case path_command_move_to:
            *t = 0.0f;
            _stage = path_command_line_to;
            _t = _step;
            break;
        case path_command_line_to:
            if(_t < 1.0f)
            {
                *t = _t;
                _t += _step;
            }
            else
            {
                *t = 1.0f;
                _stage = path_command_stop;
            }
            break;
        }
        return stage;
    }

}
