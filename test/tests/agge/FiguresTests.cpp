#include <agge/primitives/figures.h>

#include "assertex.h"
#include "mocks.h"

#include <agge/primitives/path.h>

#include <ut/test.h>

using namespace std;

namespace agge
{
	namespace tests
	{
		begin_test_suite( LineTests )
			test( LineProducesIterator )
			{
				// INIT
				real_t x, y;

				// INIT / ACT
				line l(1.0f, 13.0f, 20.0f, 17.0f);

				// ACT / ASSERT
				assert_equal(path_command_move_to, l.vertex(&x, &y));
				assert_equal(path_command_line_to, l.vertex(&x, &y));
				assert_equal(path_command_end_poly, l.vertex(&x, &y));
				assert_equal(path_command_stop, l.vertex(&x, &y));
				assert_equal(path_command_stop, l.vertex(&x, &y));
			}


			test( LineIteratorListsItsVertices )
			{
				// INIT
				line l1(10.1f, 20.2f, 30.0f, 4.7f);

				// ACT
				mocks::path::Point points1[] = { vertex(l1), vertex(l1), vertex(l1), vertex(l1), };

				// ASSERT
				mocks::path::Point reference1[] = {
					moveto(10.1f, 20.2f), lineto(30.0f, 4.7f), { 0.0f, 0.0f, path_command_end_poly }, stop(),
				};

				assert_equal(reference1, points1);

				// INIT
				line l2(11.1f, 12.2f, 13.3f, 1.7f);

				// ACT
				mocks::path::Point points2[] = { vertex(l2), vertex(l2), vertex(l2), vertex(l2), };

				// ASSERT
				mocks::path::Point reference2[] = {
					moveto(11.1f, 12.2f), lineto(13.3f, 1.7f), { 0.0f, 0.0f, path_command_end_poly }, stop(),
				};

				assert_equal(reference2, points2);
			}


			test( RewindingIteratorStartsOverWithPoints )
			{
				// INIT
				real_t x, y;
				line l(10.1f, 20.2f, 30.0f, 4.7f);

				l.vertex(&x, &y);

				// ACT
				l.rewind(0);

				// ASSERT
				assert_equal(path_command_move_to, l.vertex(&x, &y));
				assert_equal(path_command_line_to, l.vertex(&x, &y));
				assert_equal(path_command_end_poly, l.vertex(&x, &y));
				assert_equal(path_command_stop, l.vertex(&x, &y));
			}
		end_test_suite


		begin_test_suite( RectangleTests )
			test( RectangleProducesIterator )
			{
				// INIT
				real_t x, y;

				// INIT / ACT
				rectangle r(10.0f, 20.0f, 30.0f, 40.0f);

				// ACT / ASSERT
				assert_equal(path_command_move_to, r.vertex(&x, &y));
				assert_equal(path_command_line_to, r.vertex(&x, &y));
				assert_equal(path_command_line_to, r.vertex(&x, &y));
				assert_equal(path_command_line_to, r.vertex(&x, &y));
				assert_equal(path_command_end_poly | path_flag_close, r.vertex(&x, &y));
				assert_equal(path_command_stop, r.vertex(&x, &y));
				assert_equal(path_command_stop, r.vertex(&x, &y));
			}


			test( RectangleIteratorListsItsVertices )
			{
				// INIT
				rectangle r1(10.1f, 20.2f, 30.0f, 4.7f);

				// ACT
				mocks::path::Point points1[] = { vertex(r1), vertex(r1), vertex(r1), vertex(r1), vertex(r1), vertex(r1), };

				// ASSERT
				mocks::path::Point reference1[] = {
					moveto(10.1f, 20.2f), lineto(30.0f, 20.2f), lineto(30.0f, 4.7f), lineto(10.1f, 4.7f),
					{ 0.0f, 0.0f, path_command_end_poly | path_flag_close }, stop(),
				};

				assert_equal(reference1, points1);

				// INIT
				rectangle r2(11.1f, 12.2f, 13.3f, 1.7f);

				// ACT
				mocks::path::Point points2[] = { vertex(r2), vertex(r2), vertex(r2), vertex(r2), vertex(r2), vertex(r2), };

				// ASSERT
				mocks::path::Point reference2[] = {
					moveto(11.1f, 12.2f), lineto(13.3f, 12.2f), lineto(13.3f, 1.7f), lineto(11.1f, 1.7f),
					{ 0.0f, 0.0f, path_command_end_poly | path_flag_close }, stop(),
				};

				assert_equal(reference2, points2);
			}


			test( RewindingIteratorStartsOverWithPoints )
			{
				// INIT
				rectangle r(10.1f, 20.2f, 30.0f, 4.7f);

				vertex(r), vertex(r);

				// ACT
				r.rewind(0);

				// ASSERT
				mocks::path::Point points[] = { vertex(r), vertex(r), vertex(r), vertex(r), vertex(r), vertex(r), };
				mocks::path::Point reference[] = {
					moveto(10.1f, 20.2f), lineto(30.0f, 20.2f), lineto(30.0f, 4.7f), lineto(10.1f, 4.7f),
					{ 0.0f, 0.0f, path_command_end_poly | path_flag_close }, stop(),
				};

				assert_equal(reference, points);
			}
		end_test_suite


		begin_test_suite( RoundedRectangleTests )
			test( AllowsIteration )
			{
				// INIT
				real_t x, y;

				// INIT / ACT
				rounded_rectangle r(10.0f, 20.0f, 30.0f, 40.0f, 5.0f);

				// ACT / ASSERT
				int n = 0;
				int cmd;

				assert_equal(path_command_move_to, r.vertex(&x, &y));

				for (; path_command_line_to == (cmd = r.vertex(&x, &y)); ++n)
				{	}

				assert_is_true(n >= 11);
				assert_equal(path_command_end_poly | path_flag_close, cmd);
				assert_equal(path_command_stop, r.vertex(&x, &y));
				assert_equal(path_command_stop, r.vertex(&x, &y));
			}


			test( IteratorListsItsVertices )
			{
				// INIT
				real_t x, y;
				vector<Point_r> points;

				// INIT / ACT
				rounded_rectangle r1(10.0f, 20.0f, 30.0f, 35.0f, 5.0f);

				// ACT
				while ((path_command_end_poly | path_flag_close) != r1.vertex(&x, &y))
					points.push_back(create_point(x, y));

				// ASSERT
				vector<Point_r> part;
				size_t part_size = points.size() / 4;

				assert_equal(0u, points.size() % 4);

				part.assign(points.begin() + 0 * part_size, points.begin() + 1 * part_size);
				assert_equal(create_point(10.0f, 25.0f), part.front());
				assert_points_on_circle(15.0f, 25.0f, 5.0f, part);
				assert_equal(create_point(15.0f, 20.0f), part.back());

				part.assign(points.begin() + 1 * part_size, points.begin() + 2 * part_size);
				assert_equal(create_point(25.0f, 20.0f), part.front());
				assert_points_on_circle(25.0f, 25.0f, 5.0f, part);
				assert_equal(create_point(30.0f, 25.0f), part.back());

				part.assign(points.begin() + 2 * part_size, points.begin() + 3 * part_size);
				assert_equal(create_point(30.0f, 30.0f), part.front());
				assert_points_on_circle(25.0f, 30.0f, 5.0f, part);
				assert_equal(create_point(25.0f, 35.0f), part.back());

				part.assign(points.begin() + 3 * part_size, points.begin() + 4 * part_size);
				assert_equal(create_point(15.0f, 35.0f), part.front());
				assert_points_on_circle(15.0f, 30.0f, 5.0f, part);
				assert_equal(create_point(10.0f, 30.0f), part.back());

				// INIT
				points.clear();

				// INIT / ACT
				rounded_rectangle r2(0.0f, 0.0f, 40.0f, 10.0f, 3.5f);

				// ACT
				while ((path_command_end_poly | path_flag_close) != r2.vertex(&x, &y))
					points.push_back(create_point(x, y));

				// ASSERT
				part_size = points.size() / 4;

				assert_equal(0u, points.size() % 4);

				part.assign(points.begin() + 0 * part_size, points.begin() + 1 * part_size);
				assert_points_on_circle(3.5f, 3.5f, 3.5f, part);

				part.assign(points.begin() + 1 * part_size, points.begin() + 2 * part_size);
				assert_points_on_circle(36.5f, 3.5f, 3.5f, part);

				part.assign(points.begin() + 2 * part_size, points.begin() + 3 * part_size);
				assert_points_on_circle(36.5f, 6.5f, 3.5f, part);

				part.assign(points.begin() + 3 * part_size, points.begin() + 4 * part_size);
				assert_points_on_circle(3.5f, 6.5f, 3.5f, part);
			}


			test( RewindingIteratorStartsOverWithPoints )
			{
				// INIT
				real_t x, y;
				rounded_rectangle r(10.0f, 20.0f, 30.0f, 35.0f, 5.0f);

				r.vertex(&x, &y);
				r.vertex(&x, &y);
				r.vertex(&x, &y);

				// ACT
				r.rewind(0);

				// ACT / ASSERT
				assert_equal(path_command_move_to, r.vertex(&x, &y));
				assert_equal(path_command_line_to, r.vertex(&x, &y));
			}
		end_test_suite
	}
}
