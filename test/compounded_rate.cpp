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

#include <compounded_rate.h>

#include <day_counts.h>
#include <compounding_schedule.h>

#include <calendar.h>

#include <gtest/gtest.h>

#include <chrono>


using namespace coupon_schedule;

using namespace gregorian;

using namespace std::chrono;


namespace risk_free_rate
{

	TEST(compounded_rate, make_effective1)
	{
		const auto publication = calendar{
			SaturdaySundayWeekend,
			schedule{
				period{ 2020y / April / 1d, 2020y / April / 30d },
				{}
			}
		};

		EXPECT_EQ(2020y / April / 16d, make_effective(2020y / April / 23d, weeks(1), &Preceding, publication));
	}

	TEST(compounded_rate, make_effective2)
	{
		const auto publication = calendar{
			SaturdaySundayWeekend,
			schedule{
				period{ 2020y / April / 1d, 2020y / May / 31d },
				{}
			}
		};

		EXPECT_EQ(2020y / April / 24d, make_effective(2020y / May / 25d, months(1), &ModifiedPreceding, publication));
	}

	TEST(compounded_rate, make_effective3)
	{
		const auto publication = calendar{
			SaturdaySundayWeekend,
			schedule{
				period{ 2018y / December / 1d, 2019y / February / 28d },
				{ 2019y / January / 1d }, // all other holidays do not matter for this specific example
			}
		};

		EXPECT_EQ(2019y / January / 2d, make_effective(2019y / February / 1d, months(1), &ModifiedPreceding, publication));
	}


	TEST(compounded_rate, make_effective_eom2)
	{
		const auto publication = calendar{
			SaturdaySundayWeekend,
			schedule{
				period{ 2020y / February / 1d, 2020y / March / 31d },
				{},
			}
		};

		EXPECT_EQ(2020y / February / 28d, make_effective(2020y / March / 31d, months(1), &ModifiedPreceding, publication));
	}

	TEST(compounded_rate, make_effective_eom1)
	{
		const auto publication = calendar{
			SaturdaySundayWeekend,
			schedule{
				period{ 2022y / June / 1d, 2022y / July / 31d },
				{},
			}
		};

		EXPECT_EQ(2022y / June / 30d, make_effective(2022y / July / 31d, months(1), &ModifiedPreceding, publication));
	}

	// check if EOM definition in the SIX document makes sense

	TEST(compounded_rate, make_effective_x1)
	{
		// The end date falls on the last business day of the month. The
		// start date is moved to the last business day of a month.

		const auto publication = calendar{
			SaturdaySundayWeekend,
			make_SIX_holiday_schedule()
		};

		const auto maturity = 2018y / April / 30d;
		const auto term = months{ 1 };

		const auto convention = inverse_modified_following{ maturity, term };

		EXPECT_EQ(2018y / March / 29d, make_effective(maturity, term, &convention, publication));
	}

	TEST(compounded_rate, make_effective_x2)
	{
		// Unique allocation according to the money market calendar.

		const auto publication = calendar{
			SaturdaySundayWeekend,
			make_SIX_holiday_schedule()
		};

		const auto maturity = 2018y / June / 15d;
		const auto term = months{ 1 };

		const auto convention = inverse_modified_following{ maturity, term };

		EXPECT_EQ(2018y / May / 15d, make_effective(maturity, term, &convention, publication));
	}

	TEST(compounded_rate, make_effective_x3)
	{
		// Two possible start dates according to the money market
		// calendar.leading to the end date 08.10.2018.The earlier date
		// 06.09.2018 will be selected.

		const auto publication = calendar{
			SaturdaySundayWeekend,
			make_SIX_holiday_schedule()
		};

		const auto maturity = 2018y / October / 8d;
		const auto term = months{ 1 };

		const auto convention = inverse_modified_following{ maturity, term };

		EXPECT_EQ(2018y / September / 6d, make_effective(maturity, term, &convention, publication));
	}

	TEST(compounded_rate, make_effective_x4)
	{
		// Three possible start dates according to the money market
		// calendar, leading to the end date 23.04.2018.The middle date
		// 22.03.2018 will be selected.

		const auto publication = calendar{
			SaturdaySundayWeekend,
			make_SIX_holiday_schedule()
		};

		const auto maturity = 2018y / April / 23d;
		const auto term = months{ 1 };

		const auto convention = inverse_modified_following{ maturity, term };

		EXPECT_EQ(2018y / March / 22d, make_effective(maturity, term, &convention, publication));
	}

	TEST(compounded_rate, make_effective_x5)
	{
		// The previous business day is used since 10.11.2019 is not a
		// non - business day.

		// (probably typo in SIX document)

		const auto publication = calendar{
			SaturdaySundayWeekend,
			make_SIX_holiday_schedule()
		};

		const auto maturity = 2019y / December / 10d;
		const auto term = months{ 1 };

		const auto convention = inverse_modified_following{ maturity, term };

		EXPECT_EQ(2019y / November / 8d, make_effective(maturity, term, &convention, publication));
	}


	// add tests for make_maturity


	TEST(compounded_rate, compound)
	{
		const auto resets_period = period{ 2018y / April / 2d, 2018y / April / 6d };
		const auto index_period = period{ 2018y / April / 2d, 2018y / April / 9d };

		auto ts = resets::storage{ resets_period };
		ts[2018y / April / 2d] = 1.80;
		ts[2018y / April / 3d] = 1.83;
		ts[2018y / April / 4d] = 1.74;
		ts[2018y / April / 5d] = 1.75;
		ts[2018y / April / 6d] = 1.75;

		const auto r = resets{ move(ts), &Actual360 };
		const auto c = calendar{
			SaturdaySundayWeekend,
			schedule{ index_period, {} }
		};

		const auto schedule2 = make_compounding_schedule(
			{ { 2018y / April / 2d, 2018y / April / 3d }, 2018y / April / 3d, 2018y / April / 3d },
			c
		);
		EXPECT_NEAR(0.018, compound(schedule2, r), 0.000001);
		// add more steps
	}

}
