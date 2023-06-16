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

#include <rapidcsv.h>

#include <chrono>
#include <string>
#include <sstream>
#include <memory>
#include <optional>


namespace rapidcsv
{

	template<>
	inline void Converter<std::chrono::year_month_day>::ToVal(const std::string& str, std::chrono::year_month_day& val) const
	{
		auto ss = std::istringstream{ str };

		ss >> std::chrono::parse("%2d %b %2y", val); // SONIA

		if (ss.rdstate() == std::ios_base::failbit)
		{
			ss = std::istringstream{ str };

			ss >> std::chrono::parse("%Y-%2m-%2d", val); // EuroSTR
		}

		if (ss.rdstate() == std::ios_base::failbit)
		{
			ss = std::istringstream{ str };

			ss >> std::chrono::parse("%2d.%2m.%Y", val); // SARON
		}
	}

}


namespace risk_free_rate
{

	// from https://www.bankofengland.co.uk/markets/sonia-benchmark
	constexpr auto SONIA = "SONIA.csv";
	constexpr auto SONIACompoundedIndex = "SONIA_compounded_index.csv";
	// daily rounding to 18 decimal places would need more thinking


	// from https://www.ecb.europa.eu/stats/financial_markets_and_interest_rates/euro_short-term_rate/html/index.en.html
	constexpr auto EuroSTR = "EuroSTR.csv";
	constexpr auto EuroSTRCompoundedIndex = "EuroSTR_compounded_index.csv";


	// from https://www.six-group.com/en/products-services/the-swiss-stock-exchange/market-data/indices/swiss-reference-rates.html
	constexpr auto SARON = "SARON.csv";
	constexpr auto SARONCompoundedIndex = SARON;


	inline auto _make_from_until(
		const rapidcsv::Document& csv,
		const std::string dateColumnName
	) -> calendar::days_period
	{
		const auto size = csv.GetRowCount();
		if (size != 0u)
		{
			// we expect observations to be stored in decreasing order (in time)
			auto from = csv.GetCell<std::chrono::year_month_day>(dateColumnName, size - 1u);
			auto until = csv.GetCell<std::chrono::year_month_day>(dateColumnName, 0u);

			return { std::move(from), std::move(until) };
		}
		else
		{
			return { {}, {} };
		}
	}


	inline auto parse_csv(
		const std::string& fileName,
		const std::string& dateColumnName,
		const std::string& observationColumnName,
		const char separator = ','
	) -> resets::storage
	{
		const auto csv = rapidcsv::Document(
			fileName, 
			rapidcsv::LabelParams(0u, -1), // we expect titles
			rapidcsv::SeparatorParams(separator)
		);
		// at the moment we adjust the input files manually
		// to make sure we have a simple and complete single row of titles
		// (could it be done programatically here?)

		auto from_until = _make_from_until(csv, dateColumnName);

		auto ts = resets::storage{
			std::move(from_until),
		};

		const auto size = csv.GetRowCount();
		for (auto i = 0u; i < size; ++i)
		{
			// we expect each row to contain date,observation
			const auto date = csv.GetCell<std::chrono::year_month_day>(dateColumnName, i);
			const auto observation = csv.GetCell<double>(observationColumnName, i);

			ts[date] = observation;
		}

		return ts;
	}


	// from https://www.gov.uk/bank-holidays
	constexpr auto EnglandAndWalesICS = "england-and-wales.ics";

}
