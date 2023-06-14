// The MIT License (MIT)
//
// Copyright (c) 2023 Andrey Gorbachev
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <resets.h>
#include <compounded_index.h>

#include <day_counts.h>

#include <period.h>
#include <weekend.h>
#include <schedule.h>
#include <calendar.h>

#include <gtest/gtest.h>

#include <chrono>
#include <memory>


using namespace coupon_schedule;

//using namespace calendar;

using namespace std;
using namespace std::chrono;


namespace risk_free_rate
{

	TEST(sofr, make_compounded_index)
	{
		const auto resets_period = calendar::period{ 2018y / April / 2d, 2018y / April / 6d };
		const auto index_period = calendar::period{ 2018y / April / 2d, 2018y / April / 9d };

		auto ts = resets::storage{ resets_period };
		ts[2018y / April / 2d] = 1.80;
		ts[2018y / April / 3d] = 1.83;
		ts[2018y / April / 4d] = 1.74;
		ts[2018y / April / 5d] = 1.75;
		ts[2018y / April / 6d] = 1.75;

		const auto r = resets{ move(ts), &Actual360 };
		const auto from = 2018y / April / 2d;
		const auto publication = calendar::calendar{
			calendar::SaturdaySundayWeekend,
			calendar::schedule{ index_period, {} }
		};
		const auto starting_value = 1.0;
		const auto ci = make_compounded_index(
			r,
			from,
			publication,
			starting_value
		);

		auto expected = resets::storage{ index_period };
		expected[2018y / April / 2d] = 1.00000000;
		expected[2018y / April / 3d] = 1.00005000;
		expected[2018y / April / 4d] = 1.00010084;
		expected[2018y / April / 5d] = 1.00014917;
		expected[2018y / April / 6d] = 1.00019779;
		expected[2018y / April / 9d] = 1.00034365;

		EXPECT_EQ(expected, ci.get_time_series());
	}

}
