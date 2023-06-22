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

#include "inverse_modified_following.h"
#include "setup.h"

#include <resets.h>
#include <compounded_index.h>
#include <compounded_rate.h>

#include <day_counts.h>

#include <period.h>
#include <weekend.h>

#include <gtest/gtest.h>

#include <chrono>
#include <memory>
#include <calendar.h>


using namespace coupon_schedule;

//using namespace calendar;

using namespace std;
using namespace std::chrono;


namespace risk_free_rate
{

	TEST(saron, make_compounded_index2)
	{
		auto ts = parse_csv(
			SARON,
			"Date"s,
			"Swiss Average Rate ON"s,
			';'
		);

		auto hs = make_SIX_holiday_schedule();

		const auto r = resets{ move(ts), &Actual360 };
		const auto from = 1999y / June / 30d;
		const auto publication = calendar::calendar{
			calendar::SaturdaySundayWeekend,
			move(hs)
		};
		const auto decimal_places = 6u;
		const auto starting_value = 10'000.0;
		const auto ci = make_compounded_index2(
			r,
			from,
			publication,
			decimal_places,
			starting_value
		);

		const auto expected = parse_csv(
			SARONCompoundedIndex,
			"Date"s,
			"SARON Index"s,
			';'
		);
//		EXPECT_EQ(expected, ci.get_time_series());
		for (auto d = expected.get_period().get_from();
			d <= expected.get_period().get_until();
			d = sys_days{ d } + days{ 1 }
		)
		{
			const auto& o = ci.get_time_series()[d];

			const auto& e = expected[d];
			if (e)
				EXPECT_EQ(*e, *o);
			else
				EXPECT_FALSE(o);
		}
		// factor out a comparison function?
	}

	TEST(saron, SwissCurrentRateON) // better name?
	{
		auto ts = parse_csv(
			SARON,
			"Date"s,
			"Swiss Current Rate ON"s,
			';'
		);

		auto hs = make_SIX_holiday_schedule();

		const auto r = resets{ move(ts), &Actual360 };
		const auto from = 1999y / June / 30d;
		const auto publication = calendar::calendar{
			calendar::SaturdaySundayWeekend,
			move(hs)
		};
		const auto decimal_places = 6u;
		const auto starting_value = 10'000.0;
		const auto ci = make_compounded_index2(
			r,
			from,
			publication,
			decimal_places,
			starting_value
		);

		const auto expected = parse_csv(
			SARONCompoundedIndex,
			"Date"s,
			"Swiss Current Index ON"s,
			';'
		);
//		EXPECT_EQ(expected, ci.get_time_series());
		for (auto d = expected.get_period().get_from();
			d <= expected.get_period().get_until();
			d = sys_days{ d } + days{ 1 }
		)
		{
			const auto& o = ci.get_time_series()[d];

			const auto& e = expected[d];
			if (e)
				EXPECT_EQ(*e, *o);
			else
				EXPECT_FALSE(o);
		}
	}

/*
	TEST(saron, start_date_1m)
	{
		auto hs = make_SIX_holiday_schedule();
		const auto publication = calendar::calendar{
			calendar::SaturdaySundayWeekend,
			move(hs)
		};

		const auto term = months{ 1 };

		const auto csv = rapidcsv::Document(
			SARONCompoundedRate1M,
			rapidcsv::LabelParams(0u, -1), // we expect titles
			rapidcsv::SeparatorParams(';')
		);

		const auto size = csv.GetRowCount();
		for (auto i = 0u; i < size; ++i)
		{
			const auto date = csv.GetCell<std::chrono::year_month_day>("date", i);
			const auto end_date_expected = csv.GetCell<std::chrono::year_month_day>("end_date", i);
			const auto start_date_expected = csv.GetCell<std::chrono::year_month_day>("start_date", i);
			const auto day_count_expected = csv.GetCell<int>("day_count", i);

			// should we stick with effective/maturity or consider start_date/end_date instead?
			const auto end_date = make_overnight_maturity(date, publication);

			const auto convention = inverse_modified_following{ end_date, term };
			const auto start_date = make_effective(end_date, term, &convention, publication);

			const auto day_count = sys_days{ end_date } - sys_days{ start_date };

			EXPECT_EQ(end_date_expected, end_date);
			EXPECT_EQ(start_date_expected, start_date);
			EXPECT_EQ(day_count_expected, day_count.count());
		}
	}
*/
/*
	TEST(saron, make_compounded_rate_1w)
	{
		auto ts = parse_csv(
			SARON,
			"Date"s,
			"Swiss Average Rate ON"s,
			';'
		);

		auto hs = make_SIX_holiday_schedule();

		const auto term = weeks{ 1 };
		const auto r = resets{ move(ts), &Actual360 };
		const auto from = 2000y / June / 23d;
		const auto convention = &calendar::Preceding;
		const auto publication = calendar::calendar{
			calendar::SaturdaySundayWeekend,
			move(hs)
		};
		const auto decimal_places = 4u;
		const auto cr = make_compounded_rate(
			term,
			r,
			from,
			convention,
			publication,
			decimal_places
		);

		const auto expected = parse_csv(
			SARONCompoundedRate1W,
			"end_date"s,
			"value"s,
			';'
		);
//		EXPECT_EQ(expected, cr.get_time_series());
		for (auto d = expected.get_period().get_from();
			d <= expected.get_period().get_until();
			d = sys_days{ d } + days{ 1 }
		)
		{
			const auto& o = cr.get_time_series()[d];

			const auto& e = expected[d];
			if (e)
				EXPECT_EQ(*e, *o);
			else
				EXPECT_FALSE(o);
		}
	}
*/
}
