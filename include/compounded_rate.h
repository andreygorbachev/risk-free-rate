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
#include "round.h"
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

	// is this the right place for this function?
	inline auto _make_ok(const std::chrono::year_month_day& ymd) noexcept -> std::chrono::year_month_day
	{
		// could we have !ok for some other reason than expected below?
		if (ymd.ok())
			return ymd;
		else
			// we have a non-existing day of month (29, 30 or 31)
			return std::chrono::year_month_day_last{
				ymd.year(),
				ymd.month() / std::chrono::last
			};
	}


	template<typename T>
	auto make_maturity(
		const std::chrono::year_month_day& effective,
		const T& term,
		const calendar::business_day_convention* const convention,
		const calendar::calendar& publication
	) -> std::chrono::year_month_day
	{
		auto result = effective + term;

		result = _make_ok(result);

		result = convention->adjust(result, publication);

		return result;
	}

	template<>
	inline auto make_maturity(
		const std::chrono::year_month_day& effective,
		const std::chrono::weeks& term,
		const calendar::business_day_convention* const convention,
		const calendar::calendar& publication
	) -> std::chrono::year_month_day
	{
		auto result = std::chrono::year_month_day{
			std::chrono::sys_days{ effective } + term
		};

		result = convention->adjust(result, publication);

		return result;
	}

	template<typename T>
	auto make_effective(
		const std::chrono::year_month_day& maturity,
		const T& term,
		const calendar::business_day_convention* const convention,
		const calendar::calendar& publication
	) -> std::chrono::year_month_day
	{
		auto result = maturity - term;

		result = _make_ok(result);

		result = convention->adjust(result, publication);

		return result;
	}

	template<>
	inline auto make_effective(
		const std::chrono::year_month_day& maturity,
		const std::chrono::weeks& term,
		const calendar::business_day_convention* const convention,
		const calendar::calendar& publication
	) -> std::chrono::year_month_day
	{
		auto result = std::chrono::year_month_day{
			std::chrono::sys_days{ maturity } - term
		};

		result = convention->adjust(result, publication);

		return result;
	}
	// or should we do from/until instead of effective/maturity?
	// are both there functions and the overnight maturity function just an example of a relative date? (which might be better captured as a class)



	// should it be implemented via recursion as well? (so we can add one more priod if needed)
	// (this might help with the index calcuation as well)
	inline auto compound(const coupon_schedule::compounding_periods& periods, const resets& resets) -> double
	{
		const auto dc = resets.get_day_count();

		auto c = 1.0;
		for (const auto& p : periods)
			c *= 1.0 + resets[p._reset] * dc->fraction(p._period);

		const auto full_period = calendar::period{
			periods.front()._period.get_from(),
			periods.back()._period.get_until()
		};
		// does it work for degenerate compounding schedules?

		return (c - 1.0) / dc->fraction(full_period);
	}
	// maybe to move the average through time we can also undo an oldest period and then add a 1 new
	// (but would it be the same thing numerically?)

	// compounding forward or backwards - probably slightly different results numerically
	// (is it 100% clear from documentation that it sould be forward only?)



	inline auto make_compounded_rate( // should it be make_compounded_rate_resets?
		const std::chrono::months& term,
		const resets& r,
		std::chrono::year_month_day from,
		const calendar::business_day_convention* const convention,
		const calendar::calendar& publication,
		const unsigned decimal_places
	) -> resets
	{
		const auto& last_reset_ymd = r.last_reset_year_month_day();

		auto until = coupon_schedule::make_overnight_maturity(last_reset_ymd, publication);

		auto from_until = calendar::days_period{ std::move(from), std::move(until) };

		auto result = resets::storage{ std::move(from_until) };

		for (auto d = from; d <= until; d = coupon_schedule::make_overnight_maturity(d, publication))
		{
			const auto effective = make_effective(
				d,
				term,
				convention,
				publication
			);
			const auto maturity = d;

			if (effective >= from) // this also means that we can have resets "from" well in advance of actual first reset
			{
				const auto coupon_period = coupon_schedule::coupon_period{ { effective, maturity }, maturity };

				const auto schedule = coupon_schedule::make_compounding_schedule(coupon_period, publication);

				const auto rate = compound(schedule, r);

				result[maturity] = round(to_percent(rate), decimal_places);
				// from_percent/to_percent - too fragile? (should it be in the parser only?)
				// maybe resets is in %, but some view on that is what we need for calcs?
				// (also optinal in resets and NaN in the view?)
			}
		}

		const auto day_count = r.get_day_count();

		return resets{ std::move(result), day_count }; // we assume that resets day count and rate day count are the same
	}


	// needs a better name
	inline auto make_compounded_rate2( // should it be make_compounded_rate_resets?
		const std::chrono::weeks& term,
		const resets& r,
		std::chrono::year_month_day from,
		const calendar::business_day_convention* const convention,
		const calendar::calendar& publication,
		const unsigned decimal_places
	) -> resets
	{
		const auto& last_reset_ymd = r.last_reset_year_month_day();

		auto until = coupon_schedule::make_overnight_maturity(last_reset_ymd, publication);

		auto from_until = calendar::days_period{ std::move(from), std::move(until) };

		auto result = resets::storage{ std::move(from_until) };

		for (auto d = from; d <= until; d = coupon_schedule::make_overnight_maturity(d, publication))
		{
			const auto effective = make_effective(
				d,
				term,
				convention,
				publication
			);
			const auto maturity = d;

			if (effective >= from) // this also means that we can have resets "from" well in advance of actual first reset
			{
				const auto coupon_period = coupon_schedule::coupon_period{ { effective, maturity }, maturity };

				const auto schedule = coupon_schedule::make_compounding_schedule(coupon_period, publication);

				const auto rate = compound(schedule, r);

				result[maturity] = round(to_percent(rate), decimal_places);
				// from_percent/to_percent - too fragile? (should it be in the parser only?)
				// maybe resets is in %, but some view on that is what we need for calcs?
				// (also optinal in resets and NaN in the view?)
			}
		}

		const auto day_count = r.get_day_count();

		return resets{ std::move(result), day_count }; // we assume that resets day count and rate day count are the same
	}

}


