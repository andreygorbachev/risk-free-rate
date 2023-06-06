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


#include "time_series.h"

#include <period.h>

#include <fstream>
#include <vector>
#include <chrono>
#include <stdexcept>
#include <tuple>
#include <string>
#include <istream>
#include <memory>


namespace risk_free_rate
{

	// from https://www.bankofengland.co.uk/markets/sonia-benchmark
	constexpr auto SONIA = "Bank of England  Database.csv";


	inline auto _parse_csv(std::istream& fs) -> std::tuple<std::vector<std::chrono::year_month_day>, std::vector<double>>
	{
		// skip titles
		auto t = std::string{};
		std::getline(fs, t);

		auto dates = std::vector<std::chrono::year_month_day>{};
		auto observations = std::vector<double>{};

		for (;;)
		{
			// get the date
			auto ds = std::string{};
			std::getline(fs, ds, ',');

			auto d = std::chrono::year_month_day{};
			std::chrono::from_stream(fs, "%d %m %y", d);

			// get the observation
			auto os = std::string{};
			std::getline(fs, os);

			const auto o = std::stod(os);

			dates.push_back(d);
			observations.push_back(o);
		}

		return { std::move(dates), std::move(observations) };
	}

	inline auto _make_from_until(const std::vector<std::chrono::year_month_day>& dates) noexcept -> calendar::days_period
	{
		if (dates.empty())
			throw std::out_of_range{ "Dates can't be empty" };

		return { dates.front(), dates.back() };
	}


	inline auto parse_csv(const std::string& fileName) -> time_series<double>
	{
		/*const*/ auto fs = std::ifstream{ fileName };

		const auto [dates, observations] = _parse_csv(fs);

		auto from_until = _make_from_until(dates);

		return time_series<double>{
			std::move(from_until),
		};
		// at the moment unpopulated
	}

}
