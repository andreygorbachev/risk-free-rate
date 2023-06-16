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

	TEST(saron, make_compounded_index2)
	{
		auto ts = parse_csv(
			SARON,
			"Date"s,
			"Swiss Average Rate ON"s,
			';'
		);

		// from https://www.six-group.com/en/products-services/the-swiss-stock-exchange/market-data/news-tools/trading-currency-holiday-calendar.html#/
		const auto BerchtoldDay = calendar::offset_holiday<calendar::named_holiday>{ calendar::NewYearsDay, std::chrono::days{ 1 } };
		const auto LaborDay = calendar::named_holiday{ std::chrono::May / std::chrono::day{ 1u } }; // should it be in calendar?
		const auto NationalDay = calendar::named_holiday{ std::chrono::August / std::chrono::day{ 1u } };
		auto rules = std::unordered_set<const calendar::annual_holiday*>{};
		rules.insert(&calendar::NewYearsDay);
		rules.insert(&BerchtoldDay);
		rules.insert(&calendar::GoodFriday);
		rules.insert(&calendar::EasterMonday);
		rules.insert(&LaborDay);
		rules.insert(&calendar::AscensionDay);
		rules.insert(&calendar::Whitmonday);
		rules.insert(&NationalDay);
//		rules.insert(&calendar::ChristmasEve);
		rules.insert(&calendar::ChristmasDay);
		rules.insert(&calendar::BoxingDay); // should it be called it St. Stephen's Day?
//		rules.insert(&calendar::NewYearsEve);
		auto hs = calendar::make_holiday_schedule(
			{ std::chrono::year{ 1999 }, std::chrono::year{ 2024 } },
			rules
		);

		const auto r = resets{ move(ts), &Actual360 };
		const auto from = 1999y / June / 30d;
		const auto publication = calendar::calendar{
			calendar::SaturdaySundayWeekend,
			std::move(hs)
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
				EXPECT_DOUBLE_EQ(*e, *o);
			else
				EXPECT_FALSE(o);
		}
	}

	TEST(saron, SwissCurrentRateON) // better name?
	{
		auto ts = parse_csv(
			SARON,
			"Date"s,
			"Swiss Current Rate ON"s,
			';'
		);

		// from https://www.six-group.com/en/products-services/the-swiss-stock-exchange/market-data/news-tools/trading-currency-holiday-calendar.html#/
		const auto BerchtoldDay = calendar::offset_holiday<calendar::named_holiday>{ calendar::NewYearsDay, std::chrono::days{ 1 } };
		const auto LaborDay = calendar::named_holiday{ std::chrono::May / std::chrono::day{ 1u } }; // should it be in calendar?
		const auto NationalDay = calendar::named_holiday{ std::chrono::August / std::chrono::day{ 1u } };
		auto rules = std::unordered_set<const calendar::annual_holiday*>{};
		rules.insert(&calendar::NewYearsDay);
		rules.insert(&BerchtoldDay);
		rules.insert(&calendar::GoodFriday);
		rules.insert(&calendar::EasterMonday);
		rules.insert(&LaborDay);
		rules.insert(&calendar::AscensionDay);
		rules.insert(&calendar::Whitmonday);
		rules.insert(&NationalDay);
//		rules.insert(&calendar::ChristmasEve);
		rules.insert(&calendar::ChristmasDay);
		rules.insert(&calendar::BoxingDay); // should it be called it St. Stephen's Day?
//		rules.insert(&calendar::NewYearsEve);
		auto hs = calendar::make_holiday_schedule(
			{ std::chrono::year{ 1999 }, std::chrono::year{ 2024 } },
			rules
		);
		// factor this out?

		const auto r = resets{ move(ts), &Actual360 };
		const auto from = 1999y / June / 30d;
		const auto publication = calendar::calendar{
			calendar::SaturdaySundayWeekend,
			std::move(hs)
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
				EXPECT_DOUBLE_EQ(*e, *o);
			else
				EXPECT_FALSE(o);
		}
	}

}
