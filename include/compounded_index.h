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

#include "resets.h"

#include <period.h>

#include <chrono>
#include <memory>


namespace risk_free_rate
{

	inline auto round(const double x, const unsigned decimal_places) -> double
	{
		const auto p = std::pow(10.0, decimal_places);
		return std::round(x * p) / p;
	}


	inline auto make_compounded_index(
		const resets& r,
		std::chrono::year_month_day from
	) -> resets
	{
		// for now we assume that "from" exists in r (which is probably what all real cases do)

		auto index = 100.0; // for now we assume that all indices start with 100.0

		auto until = r.get_time_series().get_period().get_until(); // we should have 1 more day for the index

		auto from_until = calendar::days_period{ std::move(from), std::move(until) };

		return r; // temp only
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
