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

#include <compounded_index.h>

#include <period.h>
#include <weekend.h>
#include <schedule.h>
#include <calendar.h>

#include <gtest/gtest.h>

#include <chrono>


//using namespace calendar;

using namespace std::chrono;


namespace risk_free_rate
{

	TEST(compounded_index, make_overnight_maturity)
	{
		const auto publication = calendar::calendar{
			calendar::SaturdaySundayWeekend,
			calendar::schedule{
				calendar::period{ 2023y / May / 26d, 2023y / May / 30d },
				{ 2023y / May / 29d }
			}
		};

		EXPECT_EQ(2023y / May / 30d, make_overnight_maturity(2023y / May / 26d, publication));
	}

	TEST(compounded_index, round)
	{
		EXPECT_DOUBLE_EQ(1.01, round(1.011111, 2u));
	}

}
