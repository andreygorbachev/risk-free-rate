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

#pragma once

#include "time_series.h"
#include "resets.h"

#include <compounding_schedule.h>
#include <coupon_period.h>

#include <period.h>
#include <business_day_conventions.h>
#include <calendar.h>

#include <chrono>
#include <memory>


namespace risk_free_rate
{

	template<typename T>
	auto make_effective(
		const std::chrono::year_month_day& maturity,
		const calendar::calendar& publication,
		const int offset // good name?
	) -> std::chrono::year_month_day
	{
		return calendar::ModifiedPreceding.adjust(
			maturity - T{ offset },
			publication
		); // please note that 1w version needs Preceding
	}
	// or should we do from/until instead of effective/maturity?
	// are both there functions just an example of a relative date? (which might be better captured as a class)


	inline auto make_compounded_rate(
		const resets& r,
		std::chrono::year_month_day from,
		const calendar::calendar& publication
	) -> resets
	{
		const auto& last_reset_ymd = r.get_time_series().get_period().get_until();

		const auto effective = make_effective<std::chrono::months>(last_reset_ymd, publication, 3);
		const auto maturity = last_reset_ymd;
		// at the moment a single rate, only for 3m

		const auto coupon_period = coupon_schedule::coupon_period{ { effective, maturity }, maturity };

		const auto schedule = coupon_schedule::make_compounding_schedule(coupon_period, publication);

		return r; // temp only
	}

}
