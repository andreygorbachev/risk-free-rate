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

#include <period.h>
#include <business_day_conventions.h>
#include <calendar.h>

#include <chrono>
#include <memory>


namespace risk_free_rate
{

	inline auto round(const double x, const unsigned decimal_places) -> double
	{
		const auto p = std::pow(10.0, decimal_places);
		return std::round(x * p) / p;
	}


	inline auto make_overnight_maturity(
		const std::chrono::year_month_day& effective,
		const calendar::calendar& publication
	) -> std::chrono::year_month_day
	{
		return calendar::Following.adjust(
			std::chrono::sys_days{ effective } + std::chrono::days{ 1 },
			publication
		);
	}


	inline auto make_compounded_index(
		const resets& r,
		std::chrono::year_month_day from,
		const calendar::calendar& publication,
		const unsigned decimal_places,
		const double starting_value = 100.0 // alternatively we can rebalance everything for 1.0
	) -> resets
	{
		// for now we assume that "from" exists in r (which is probably what all real cases do)

		// at the moment we do not protect against resets (incorrectly) provided for non-business days
		// (they are just ignored in these calculations)
		// is this an issue for the last reset?

		const auto last_reset_ymd = r.get_time_series().get_period().get_until();

		// resets are stored based on effective date of the rate (not a publication date, which is the next business day)
		// but compounded index is published for the maturity of the last rate participating in the calculation of the index
		// hence we need to use publication_calendar to add 1 business day to the latest reset date
		auto until = make_overnight_maturity(last_reset_ymd, publication);

		auto from_until = calendar::days_period{ std::move(from), std::move(until) };

		auto result = resets::storage{ std::move(from_until) };

		const auto day_count = r.get_day_count();

		auto index = starting_value;
		result[from] = index;

		for (auto d = from; d <= last_reset_ymd;)
		{
			const auto effective = d;
			const auto maturity = make_overnight_maturity(d, publication);
			const auto year_fraction = day_count->fraction({ effective, maturity });

			index *= 1.0 + r[effective] * year_fraction;

			// I need to find a better way of handling "not a rate" resets (at the moment we mix together rates and indices, which is not clean)
			result[maturity] = round(index, decimal_places);
			// I also read it as "only the final result is rounded" (no rounding on each step of the calculation)

			d = maturity;
		}

		return resets{ std::move(result), day_count }; // we assume that resets day count and index day count are the same
	}


	class compounded_index
	{

	public:

		explicit compounded_index(std::chrono::year_month_day from);

	public:

		auto value(const std::chrono::year_month_day& ymd) const -> double; // noexcept?

	private:

		std::chrono::year_month_day _from;

		// we should be able to cache results here

	};



	inline compounded_index::compounded_index(std::chrono::year_month_day from) : _from{ std::move(from) }
	{
	}


	inline auto compounded_index::value(const std::chrono::year_month_day& ymd) const -> double
	{
		return 100.0; // temp only
	}

}
