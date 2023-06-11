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

#include <day_count_interface.h>

#include <cmath>
#include <memory>


namespace risk_free_rate
{

	class resets
	{

	public:

		using storage = time_series<double>; // or should we consider some ratio? (s.t. rounding would be explicit)

	public:

		resets() noexcept = delete;
		resets(const resets&) = default;
		resets(resets&&) noexcept = default;

		~resets() noexcept = default;

		resets& operator=(const resets&) = default;
		resets& operator=(resets&&) noexcept = default;

		explicit resets(storage ts, const coupon_schedule::day_count* dc);

	public:

		auto get_time_series() const noexcept -> const storage&;
		auto get_day_count() const noexcept -> const coupon_schedule::day_count*;

	private:

		storage _ts;

		const coupon_schedule::day_count* _dc;

	};



	inline resets::resets(storage ts, const coupon_schedule::day_count* dc) :
		_ts{ std::move(ts) },
		_dc{ dc }
	{
	}



	inline auto resets::get_time_series() const noexcept -> const storage&
	{
		return _ts;
	}

	inline auto resets::get_day_count() const noexcept -> const coupon_schedule::day_count*
	{
		return _dc;
	}

}
