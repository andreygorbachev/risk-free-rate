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
#include <weekend.h>
#include <schedule.h>
#include <calendar.h>
#include <annual_holidays.h>

#include <rapidcsv.h>

#include <chrono>
#include <string>
#include <sstream>
#include <memory>
#include <optional>
#include <unordered_set>
#include <cmath>


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

	template<>
	inline void Converter<std::optional<double>>::ToVal(const std::string& str, std::optional<double>& val) const
	{
		if(!str.empty())
			val = std::stod(str); // should probably check pos as well
		else
			val = std::nullopt;
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
	constexpr auto EuroSTRCompoundedRate = EuroSTRCompoundedIndex;


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
			rapidcsv::SeparatorParams(separator),
			rapidcsv::ConverterParams(true) // needed for EuroSTR compounded rates
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
			const auto observation = csv.GetCell<std::optional<double>>(observationColumnName, i);

			if(observation)
				ts[date] = observation; // or should [] work with optional<double>, rather than double? (some functionality for both?)
		}

		return ts;
	}


	// from https://www.gov.uk/bank-holidays
	constexpr auto EnglandAndWalesICS = "england-and-wales.ics";


	inline auto make_TARGET2_holiday_schedule() -> calendar::schedule
	{
		// from https://www.ecb.europa.eu/paym/target/target2/profuse/calendar/html/index.en.html

		const auto LaborDay = calendar::named_holiday{ std::chrono::May / std::chrono::day{ 1u } }; // should it be in calendar?

		auto rules = std::unordered_set<const calendar::annual_holiday*>{};
		rules.insert(&calendar::NewYearsDay);
		rules.insert(&calendar::GoodFriday);
		rules.insert(&calendar::EasterMonday);
		rules.insert(&LaborDay);
		rules.insert(&calendar::ChristmasDay);
		rules.insert(&calendar::BoxingDay);

		return calendar::make_holiday_schedule(
			{ std::chrono::year{ 2002 }, std::chrono::year{ 2023 } },
			rules
		);
	}


	inline auto make_SIX_holiday_schedule() -> calendar::schedule
	{
		// from https://www.six-group.com/en/products-services/the-swiss-stock-exchange/market-data/news-tools/trading-currency-holiday-calendar.html#/

		const auto BerchtoldDay = calendar::offset_holiday<calendar::named_holiday>{ calendar::NewYearsDay, std::chrono::days{ 1 } };
		const auto LaborDay = calendar::named_holiday{ std::chrono::May / std::chrono::day{ 1u } }; // should it be in calendar?
		const auto NationalDay = calendar::named_holiday{ std::chrono::August / std::chrono::day{ 1u } };

		auto rules = std::unordered_set<const calendar::annual_holiday*>{};
		rules.insert(&calendar::NewYearsDay);
		rules.insert(&BerchtoldDay);
		rules.insert(&calendar::GoodFriday);
		rules.insert(&calendar::EasterMonday);
		rules.insert(&LaborDay);
		rules.insert(&calendar::AscensionDay);
		rules.insert(&calendar::Whitmonday);
		rules.insert(&NationalDay);
//		rules.insert(&calendar::ChristmasEve);
		rules.insert(&calendar::ChristmasDay);
		rules.insert(&calendar::BoxingDay); // should it be called it St. Stephen's Day?
//		rules.insert(&calendar::NewYearsEve);

		return calendar::make_holiday_schedule(
			{ std::chrono::year{ 1999 }, std::chrono::year{ 2024 } },
			rules
		);
	}

}
