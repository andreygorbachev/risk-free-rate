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
#include <annual_holidays.h>
#include <parser.h>

#include <gtest/gtest.h>

#include <chrono>
#include <memory>
#include <unordered_set>


using namespace coupon_schedule;

//using namespace calendar;

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

		// from https://www.ecb.europa.eu/press/pr/date/2000/html/pr001214_4.en.html
		const auto LaborDay = calendar::named_holiday{ std::chrono::May / std::chrono::day{ 1u } };
		auto rules = std::unordered_set<const calendar::annual_holiday*>{};
		rules.insert(&calendar::NewYearsDay);
		rules.insert(&calendar::GoodFriday);
		rules.insert(&calendar::EasterMonday);
		rules.insert(&LaborDay);
		rules.insert(&calendar::ChristmasDay);
		rules.insert(&calendar::BoxingDay);
		auto hs = calendar::make_holiday_schedule(
			{ std::chrono::year{ 2019 }, std::chrono::year{ 2023 } },
			rules
		);

		const auto r = resets{ move(ts), &Actual360 };
		const auto from = 2019y / October / 1d;
		const auto publication = calendar::calendar{
			calendar::SaturdaySundayWeekend,
			std::move(hs)
		};
		const auto ci = make_compounded_index(
			r,
			from,
			publication
		);

		const auto expected = parse_csv(
			EuroSTRCompoundedIndex,
			"Period"s,
			"Compounded Euro Short-Term Rate Index, Index of compounded interest"s
		);
		for (auto d = expected.get_period().get_from();
			d <= expected.get_period().get_until();
			d = sys_days{ d } + days{ 1 }
		)
		{
			const auto& o = ci.get_time_series()[d];

			const auto& e = expected[d];
			if (e)
				EXPECT_DOUBLE_EQ(*e, *o);
			else
				EXPECT_FALSE(o);
		}
	}

}