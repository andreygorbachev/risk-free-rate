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
#include <time_series.h>
#include <weekend.h>
#include <schedule.h>
#include <calendar.h>

#include <gtest/gtest.h>

#include <chrono>
#include <memory>


using namespace coupon_schedule;

using namespace gregorian;

using namespace std;
using namespace std::chrono;


namespace risk_free_rate
{

	TEST(sonia, make_compounded_index)
	{
		auto ts = parse_csv(
			SONIA,
			"Date"s,
			"Daily Sterling overnight index average (SONIA) rate              [a] [b]             IUDSOIA"s
		);

		const auto r = resets{ move(ts), &Actual365Fixed };
		const auto from = 2018y / April / 23d;
		auto publication = calendar{
			SaturdaySundayWeekend,
			make_england_holiday_schedule()
		};
		publication.substitute(&Following);
		const auto decimal_places = 8u;
		const auto ci = make_compounded_index(
			r,
			from,
			publication,
			decimal_places
		);

		const auto expected = parse_csv(
			SONIACompoundedIndex,
			"Date"s,
			"SONIA Compounded Index              [a] [b] [c] [d]             IUDZOS2"s
		);
		for (auto d = expected.get_period().get_from();
			d <= expected.get_period().get_until();
			d = sys_days{ d } + days{ 1 }
		)
		{
			const auto& o = ci.get_time_series()[d];

			const auto& e = expected[d];
			// On 14 February 2023 the Bank became aware of a small discrepancy in the calculation
			// of the SONIA Compounded Index published on that day.As the Bank became aware of this
			// discrepancy after the republication deadline of noon on 14 February, the SONIA Compounded
			// Index was not republished.To ensure the integrity of the index, and in line with the
			// definition of the SONIA Compounded Index set out in the SONIA Key Features& Policies,
			// the Bank corrected the index calculation for future dates.
			if (d != 2023y / February / 14d)
				if (e)
					EXPECT_EQ(*e, *o);
				else
					EXPECT_FALSE(o);
			else
				; // we can check the generated number against the manual calcualation for this day
		}
	}

}
