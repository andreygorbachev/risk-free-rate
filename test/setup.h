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

#include <period.h>

#include <rapidcsv.h>

#include <chrono>
#include <string>
#include <sstream>
#include <memory>


namespace rapidcsv
{

	template<>
	void Converter<std::chrono::year_month_day>::ToVal(const std::string& str, std::chrono::year_month_day& val) const
	{
		std::istringstream ss{ str };

		ss >> std::chrono::parse("%2d %b %2y", val);
	}

}


namespace risk_free_rate
{

	// from https://www.bankofengland.co.uk/markets/sonia-benchmark
	constexpr auto SONIA = "SONIA.csv";
	constexpr auto SONIACompoundedIndex = "SONIA_compounded_index.csv";
	// daily rounding to 18 decimal places would need more thinking


	inline auto _make_from_until(const rapidcsv::Document& csv) -> calendar::days_period
	{
		// we expect the observations to be stored in decreasing in time order
		auto from = csv.GetCell<std::chrono::year_month_day>(0u, csv.GetRowCount() - 1u);
		auto until = csv.GetCell<std::chrono::year_month_day>(0u, 0u);

		return { std::move(from), std::move(until) };
	}


	inline auto parse_csv(const std::string& fileName) -> time_series<double>
	{
		const auto csv = rapidcsv::Document(fileName);

		auto from_until = _make_from_until(csv);

		auto ts = time_series<double>{
			std::move(from_until),
		};

		for (auto i = 0u; i < csv.GetRowCount(); ++i)
		{
			const auto date = csv.GetCell<std::chrono::year_month_day>(0u, i);
			const auto observation = csv.GetCell<double>(1u, i);
			ts[date] = observation;
		}

		return ts;
	}

}
