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
#include <compounded_rate.h>

#include <day_counts.h>

#include <period.h>
#include <time_series.h>
#include <weekend.h>
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

	TEST(eurostr, make_compounded_index)
	{
		auto ts = parse_csv(
			EuroSTR,
			"Period"s,
			"Volume-weighted trimmed mean rate"s
		);

		auto hs = make_TARGET2_holiday_schedule();

		const auto r = resets{ move(ts), &Actual360 };
		const auto from = 2019y / October / 1d;
		const auto publication = calendar{
			SaturdaySundayWeekend,
			move(hs)
		};
		const auto decimal_places = 8u;
		const auto ci = make_compounded_index(
			r,
			from,
			publication,
			decimal_places
		);

		const auto expected = parse_csv(
			EuroSTRCompoundedIndex,
			"Period"s,
			"Compounded Euro Short-Term Rate Index, Index of compounded interest"s
		);
		EXPECT_EQ(expected, ci.get_time_series());
	}


	TEST(eurostr, make_compounded_rate_1w)
	{
		auto ts = parse_csv(
			EuroSTR,
			"Period"s,
			"Volume-weighted trimmed mean rate"s
		);

		auto hs = make_TARGET2_holiday_schedule();

		const auto term = weeks{ 1 };
		const auto r = resets{ move(ts), &Actual360 };
		const auto from = 2019y / October / 1d;
		const auto convention = &Preceding;
		const auto publication = calendar{
			SaturdaySundayWeekend,
			move(hs)
		};
		const auto decimal_places = 5u;
		const auto cr = make_compounded_rate(
			term,
			r,
			from,
			convention,
			publication,
			decimal_places
		);

		const auto expected = parse_csv(
			EuroSTRCompoundedRate,
			"Period"s,
			"Euro Short-Term Rate - 1-week Compounded Average Rate, Compounded average rate"s
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


	TEST(eurostr, make_compounded_rate_1m)
	{
		auto ts = parse_csv(
			EuroSTR,
			"Period"s,
			"Volume-weighted trimmed mean rate"s
		);

		auto hs = make_TARGET2_holiday_schedule();

		const auto term = months{ 1 };
		const auto r = resets{ move(ts), &Actual360 };
		const auto from = 2019y / October / 1d;
		const auto convention = &ModifiedPreceding;
		const auto publication = calendar{
			SaturdaySundayWeekend,
			move(hs)
		};
		const auto decimal_places = 5u;
		const auto cr = make_compounded_rate(
			term,
			r,
			from,
			convention,
			publication,
			decimal_places
		);

		const auto expected = parse_csv(
			EuroSTRCompoundedRate,
			"Period"s,
			"Euro Short-Term Rate - 1-month Compounded Average Rate, Compounded average rate"s
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


	TEST(eurostr, make_compounded_rate_3m)
	{
		auto ts = parse_csv(
			EuroSTR,
			"Period"s,
			"Volume-weighted trimmed mean rate"s
		);

		auto hs = make_TARGET2_holiday_schedule();

		const auto term = months{ 3 };
		const auto r = resets{ move(ts), &Actual360 };
		const auto from = 2019y / October / 1d;
		const auto convention = &ModifiedPreceding;
		const auto publication = calendar{
			SaturdaySundayWeekend,
			move(hs)
		};
		const auto decimal_places = 5u;
		const auto cr = make_compounded_rate(
			term,
			r,
			from,
			convention,
			publication,
			decimal_places
		);

		const auto expected = parse_csv(
			EuroSTRCompoundedRate,
			"Period"s,
			"Euro Short-Term Rate - 3-months Compounded Average Rate, Compounded average rate"s
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


	TEST(eurostr, make_compounded_rate_6m)
	{
		auto ts = parse_csv(
			EuroSTR,
			"Period"s,
			"Volume-weighted trimmed mean rate"s
		);

		auto hs = make_TARGET2_holiday_schedule();

		const auto term = months{ 6 };
		const auto r = resets{ move(ts), &Actual360 };
		const auto from = 2019y / October / 1d;
		const auto convention = &ModifiedPreceding;
		const auto publication = calendar{
			SaturdaySundayWeekend,
			move(hs)
		};
		const auto decimal_places = 5u;
		const auto cr = make_compounded_rate(
			term,
			r,
			from,
			convention,
			publication,
			decimal_places
		);

		const auto expected = parse_csv(
			EuroSTRCompoundedRate,
			"Period"s,
			"Euro Short-Term Rate - 6-months Compounded Average Rate, Compounded average rate"s
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


	TEST(eurostr, make_compounded_rate_12m)
	{
		auto ts = parse_csv(
			EuroSTR,
			"Period"s,
			"Volume-weighted trimmed mean rate"s
		);

		auto hs = make_TARGET2_holiday_schedule();

		const auto term = months{ 12 };
		const auto r = resets{ move(ts), &Actual360 };
		const auto from = 2019y / October / 1d;
		const auto convention = &ModifiedPreceding;
		const auto publication = calendar{
			SaturdaySundayWeekend,
			move(hs)
		};
		const auto decimal_places = 5u;
		const auto cr = make_compounded_rate(
			term,
			r,
			from,
			convention,
			publication,
			decimal_places
		);

		const auto expected = parse_csv(
			EuroSTRCompoundedRate,
			"Period"s,
			"Euro Short-Term Rate - 12-months Compounded Average Rate, Compounded average rate"s
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

}
