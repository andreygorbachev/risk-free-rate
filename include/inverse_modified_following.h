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

#include "compounded_rate.h" // is this the right way around?

#include <compounding_schedule.h>

#include <business_day_convention_interface.h>
#include <calendar.h>

#include <chrono>
#include <memory>
#include <vector>


namespace risk_free_rate
{

	// is this something specific to SARON's compounded rates? (if yes should the name reflect that?)
	class inverse_modified_following final : public calendar::business_day_convention
	{

	public:

		explicit inverse_modified_following(
			std::chrono::year_month_day maturity,
			std::chrono::months term // only months to start with
		) noexcept;

	private:

		virtual auto _adjust(const std::chrono::year_month_day& ymd, const calendar::calendar& cal) const noexcept -> std::chrono::year_month_day final;

	private:

		std::chrono::year_month_day _maturity;
		std::chrono::months _term;

	};


	inline inverse_modified_following::inverse_modified_following(
		std::chrono::year_month_day maturity,
		std::chrono::months term
	) noexcept :
		_maturity{ std::move(maturity) },
		_term{ std::move(term) }
	{
	}


	inline auto _middle(const std::vector<std::chrono::year_month_day>& es) noexcept -> std::chrono::year_month_day
	{
		// For each end date with several possible start dates according to the CHF money market calendar, the following 
		// applies(unless the end date is the last business day of a month :
		// – In case of an uneven number of possible start dates, the middle date will be chosen as the start date
		// – In case of an even number of possible start dates, the earlier of the two middle dates will be chosen
		const auto m = es.size() % 2;
		if (m != 0)
			return es[es.size() / 2];
		else
			return es[es.size() / 2 - 1];
	}


	// are we relying on consistency of how ymd was calculated and _maturity/_term, etc?
	inline auto inverse_modified_following::_adjust(const std::chrono::year_month_day& ymd, const calendar::calendar& cal) const noexcept -> std::chrono::year_month_day
	{
		// If the end date falls on the last business day of a month, the start date must also be the last business day of a month.
		if (_maturity == cal.last_business_day({ _maturity.year(), _maturity.month() }))
			return cal.last_business_day({ ymd.year(), ymd.month() });

		// does it cover all possibilities? (also assuming non SIX calendars)
		// could it be speed up / simplified?
		auto es = std::vector<std::chrono::year_month_day>{};
		for (auto d = std::chrono::sys_days{ ymd } - std::chrono::days{ 4 };
			d <= std::chrono::sys_days{ ymd } + std::chrono::days{ 4 };
			d += std::chrono::days{ 1 }
		)
		{
			if(cal.is_business_day(d))
				if(make_maturity(d, _term, &calendar::ModifiedFollowing, cal) == _maturity)
					es.push_back(d);
		}

		if (es.empty())
			// If the originally calculated start date falls on a non - business day or non - existent date(e.g. 30th of February), the
			// business day preceding the calculated start date will be the used as the start date, unless this new start date
			// would fall in a different month.In this case, the following business day will be used as the start date and not the
			// previous business day.
			return calendar::ModifiedPreceding.adjust(ymd, cal);
		else
			if (es.size() == 1u)
				// If the date is unique according to the CHF money market calendar, it will be used as the start date.
				return es.front();
			else
				return _middle(es);
	}
	// should we deal with serial dates?

}
