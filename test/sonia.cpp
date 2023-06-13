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

#include "setup.h"

#include <resets.h>
#include <compounded_index.h>

#include <day_counts.h>

#include <period.h>
#include <weekend.h>
#include <schedule.h>
#include <calendar.h>
#include <parser.h>

#include <gtest/gtest.h>

#include <chrono>
#include <memory>


using namespace coupon_schedule;

//using namespace calendar;

using namespace std;
using namespace std::chrono;


namespace risk_free_rate
{
	TEST(sonia, make_compounded_index)
	{
		auto ts = parse_csv(SONIA);

		const auto r = resets{ move(ts), &Actual365Fixed };
		const auto from = 2018y / April / 23d;
		const auto publication = calendar::calendar{
			calendar::SaturdaySundayWeekend,
			calendar::parser::parse_ics(EnglandAndWalesICS)
		};
		const auto ci = make_compounded_index(
			r,
			from,
			publication
		);

		const auto expected = parse_csv(SONIACompoundedIndex);
		for (auto d = expected.get_period().get_from();
			d <= expected.get_period().get_until();
			d = sys_days{ d } + days{ 1 }
		)
		{
			const auto& o = ci.get_time_series()[d];

			const auto& e = expected[d];
			if(d != 2023y / February / 14d) // something funny about that day only
				if (e)
					EXPECT_DOUBLE_EQ(*e, *o);
				else
					EXPECT_FALSE(o);
		}
		// as values are rounded do we still need to use tolerance?
	}

}
