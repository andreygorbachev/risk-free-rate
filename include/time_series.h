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

#include <period.h>

#include <vector>
#include <memory>


namespace risk_free_rate
{

	template<typename T>
	class time_series
	{

	public:

		explicit time_series(calendar::days_period period) noexcept;

	private:

		auto _index(const std::chrono::year_month_day& ymd) const -> std::size_t;

	private:

		calendar::days_period _period; // or should we consider not just daily resets?

		std::vector<T> _observations;

	};



	template<typename T>
	time_series<T>::time_series(calendar::days_period period) noexcept :
		_period{ std::move(period) },
		_observations(_index(_period.get_until()) + 1/*uz*/)
	{
	}


	template<typename T>
	auto time_series<T>::_index(const std::chrono::year_month_day& ymd) const -> std::size_t
	{
		if (ymd < _period.get_from() || ymd > _period.get_until())
			throw std::out_of_range{ "Request is not consistent with from/until" };

		const auto days = std::chrono::sys_days{ ymd } - std::chrono::sys_days{ _period.get_from() };
		return days.count();
	}

}
