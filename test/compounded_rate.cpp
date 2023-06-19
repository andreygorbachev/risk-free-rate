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

#include <compounded_rate.h>

#include <calendar.h>

#include <gtest/gtest.h>

#include <chrono>


//using namespace calendar;

using namespace std::chrono;


namespace risk_free_rate
{
/*
	TEST(compounded_rate, make_effective1)
	{
		const auto publication = calendar::calendar{
			calendar::SaturdaySundayWeekend,
			calendar::schedule{
				calendar::period{ 2020y / April / 1d, 2020y / April / 30d },
				{}
			}
		};

		EXPECT_EQ(2020y / April / 16d, make_effective<std::chrono::weeks>(2020y / April / 23d, publication, 1));
	}
*/
	TEST(compounded_rate, make_effective2)
	{
		const auto publication = calendar::calendar{
			calendar::SaturdaySundayWeekend,
			calendar::schedule{
				calendar::period{ 2020y / April / 1d, 2020y / May / 31d },
				{}
			}
		};

		EXPECT_EQ(2020y / April / 24d, make_effective<std::chrono::months>(2020y / May / 25d, publication, 1));
	}

	TEST(compounded_rate, make_effective3)
	{
		const auto publication = calendar::calendar{
			calendar::SaturdaySundayWeekend,
			calendar::schedule{
				calendar::period{ 2018y / December / 1d, 2019y / February / 28d },
				{ 2019y / January / 1d }, // all other holidays do not matter for this specific example
			}
		};

		EXPECT_EQ(2019y / January / 2d, make_effective<std::chrono::months>(2019y / February / 1d, publication, 1));
	}

}