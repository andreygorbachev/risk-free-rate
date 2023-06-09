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

#include <fstream>
#include <vector>
#include <chrono>
#include <stdexcept>
#include <utility>
#include <string>
#include <istream>
#include <memory>
#include <cstddef>


namespace risk_free_rate
{

	// from https://www.bankofengland.co.uk/markets/sonia-benchmark
	constexpr auto SONIA = "SONIA.csv";
	constexpr auto SONIACompoundedIndex = "SONIA_compounded_index.csv";
	// daily rounding to 18 decimal places would need more thinking

	using DateObservation = std::pair<std::chrono::year_month_day, double>;

	inline auto _parse_csv(std::istream& fs) -> std::vector<DateObservation>
	{
		using namespace std;

		// skip titles
		auto t = string{};
		getline(fs, t);

		auto dates_observations = vector<DateObservation>{};

		while (!fs.eof())
		{
			// get the date
			auto d = chrono::year_month_day{};
			chrono::from_stream(fs, "\"%2d %b %2y\",", d); // this is locale dependent - so needs more work

			// get the observation
			auto o = string{};
			getline(fs, o);
			// please note that o stars and ends with ", so lets remove them
//			o = o.substr(1uz, o.length() - 2uz);
			o = o.substr(1u, o.length() - 2u);

			dates_observations.emplace_back(move(d), stod(o));
		}

		return dates_observations;
	}

	inline auto _make_from_until(const std::vector<DateObservation>& dates_observations) -> calendar::days_period
	{
		if (dates_observations.empty())
			throw std::out_of_range{ "Dates/observations can't be empty" };

		return { dates_observations.front().first, dates_observations.back().first };
	}


	inline auto parse_csv(const std::string& fileName) -> time_series<double>
	{
		/*const*/ auto fs = std::ifstream{ fileName };

		const auto dates_observations = _parse_csv(fs); // or we can use one of the existing packages - why standard library does not have it?

		auto from_until = _make_from_until(dates_observations);

		auto ts = time_series<double>{
			std::move(from_until),
		};

		for (const auto& d_o : dates_observations)
			ts[d_o.first] = d_o.second;

		return ts;
	}

}
