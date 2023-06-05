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

#include <time_series.h>

#include <period.h>

#include <gtest/gtest.h>

#include <chrono>
#include <cmath>
#include <stdexcept>


using namespace std;
using namespace std::chrono;


namespace risk_free_rate
{

	TEST(time_series, constructor)
	{
		const auto ts = time_series<double>{ { 2023y / January / 1d, 2023y / June / 5d } };

		EXPECT_TRUE(isnan(ts[2023y / January / 1d]));
	}

	TEST(time_series, operator_square_brackets)
	{
		const auto ts = time_series<double>{ { 2023y / January / 1d, 2023y / June / 5d } };

		EXPECT_TRUE(isnan(ts[2023y / January / 1d]));
		EXPECT_TRUE(isnan(ts[2023y / June / 5d]));

		EXPECT_THROW(ts[2023y / June / 6d], out_of_range);
	}

}
