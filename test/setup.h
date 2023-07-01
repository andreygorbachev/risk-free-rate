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

#include <resets.h>

#include <period.h>
#include <time_series.h>
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
#include <cmath>

using namespace gregorian;

using namespace std;
using namespace std::chrono;


namespace rapidcsv
{

	template<>
	inline void Converter<year_month_day>::ToVal(const string& str, year_month_day& val) const
	{
		auto ss = istringstream{ str };

		ss >> parse("%2d %b %2y", val); // SONIA

		if (ss.rdstate() == ios_base::failbit)
		{
			ss = istringstream{ str };

			ss >> parse("%Y-%2m-%2d", val); // EuroSTR
		}

		if (ss.rdstate() == ios_base::failbit)
		{
			ss = istringstream{ str };

			ss >> parse("%2d.%2m.%Y", val); // SARON
		}
	}

	template<>
	inline void Converter<optional<double>>::ToVal(const string& str, optional<double>& val) const
	{
		if(!str.empty())
			val = stod(str); // should probably check pos as well
		else
			val = nullopt;
	}

}


namespace risk_free_rate
{

	// from https://www.bankofengland.co.uk/markets/sonia-benchmark
	constexpr auto SONIA = "SONIA.csv";
	constexpr auto SONIACompoundedIndex = "SONIA_compounded_index.csv";
	// as far as I know there are no compounded rates published for SONIA
	// daily rounding to 18 decimal places would need more thinking


	// from https://www.ecb.europa.eu/stats/financial_markets_and_interest_rates/euro_short-term_rate/html/index.en.html
	constexpr auto EuroSTR = "EuroSTR.csv";
	constexpr auto EuroSTRCompoundedIndex = "EuroSTR_compounded_index.csv";
	constexpr auto EuroSTRCompoundedRate = EuroSTRCompoundedIndex;


	// from https://www.six-group.com/en/products-services/the-swiss-stock-exchange/market-data/indices/swiss-reference-rates.html
	constexpr auto SARON = "SARON.csv";
	constexpr auto SARONCompoundedIndex = SARON;
	constexpr auto SARONCompoundedRate1W = "h_sar1wc_delayed.csv";
	constexpr auto SARONCompoundedRate1M = "h_sar1mc_delayed.csv";
	constexpr auto SARONCompoundedRate2M = "h_sar2mc_delayed.csv";
	constexpr auto SARONCompoundedRate3M = "h_sar3mc_delayed.csv";
	constexpr auto SARONCompoundedRate6M = "h_sar6mc_delayed.csv";
	constexpr auto SARONCompoundedRate9M = "h_sar9mc_delayed.csv";
	constexpr auto SARONCompoundedRate12M = "h_sar12mc_delayed.csv";


	inline auto _make_from_until(
		const rapidcsv::Document& csv,
		const string dateColumnName
	) -> gregorian::days_period
	{
		const auto size = csv.GetRowCount();
		if (size != 0u)
		{
			// we expect observations to be stored in decreasing order (in time)
			auto from = csv.GetCell<year_month_day>(dateColumnName, size - 1u);
			auto until = csv.GetCell<year_month_day>(dateColumnName, 0u);

			return { move(from), move(until) };
		}
		else
		{
			return { {}, {} };
		}
	}


	inline auto parse_csv(
		const string& fileName,
		const string& dateColumnName,
		const string& observationColumnName,
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
			move(from_until),
		};

		const auto size = csv.GetRowCount();
		for (auto i = 0u; i < size; ++i)
		{
			// we expect each row to contain date,observation
			const auto date = csv.GetCell<year_month_day>(dateColumnName, i);
			const auto observation = csv.GetCell<optional<double>>(observationColumnName, i);

			if(observation)
				ts[date] = observation; // or should [] work with optional<double>, rather than double? (some functionality for both?)
		}

		return ts;
	}


	inline auto make_england_holiday_schedule() -> schedule
	{
		// from https://www.gov.uk/bank-holidays

		const auto EarlyMayBankHoliday = weekday_indexed_holiday{ std::chrono::May / std::chrono::Monday[1] };
		const auto SpringBankHoliday = weekday_last_holiday{ std::chrono::May / std::chrono::Monday[std::chrono::last] };
		const auto SummerBankHoliday = weekday_last_holiday{ std::chrono::August / std::chrono::Monday[std::chrono::last] };

		auto rules = annual_holiday_storage{};
		rules.insert(&NewYearsDay);
		rules.insert(&GoodFriday);
		rules.insert(&EasterMonday);
		rules.insert(&EarlyMayBankHoliday);
		rules.insert(&SpringBankHoliday);
		rules.insert(&SummerBankHoliday);
		rules.insert(&ChristmasDay);
		rules.insert(&BoxingDay);

		const auto hs2018_2019 = make_holiday_schedule(
			{ std::chrono::year{ 2018 }, std::chrono::year{ 2019 } },
			rules
		);

		const auto VictoryInEuropeDay = named_holiday{ std::chrono::May / std::chrono::day{ 8u } };

		auto rules2020 = annual_holiday_storage{};
		rules2020.insert(&NewYearsDay);
		rules2020.insert(&GoodFriday);
		rules2020.insert(&EasterMonday);
		rules2020.insert(&VictoryInEuropeDay);
		rules2020.insert(&SpringBankHoliday);
		rules2020.insert(&SummerBankHoliday);
		rules2020.insert(&ChristmasDay);
		rules2020.insert(&BoxingDay);

		const auto hs2020 = make_holiday_schedule(
			std::chrono::year{ 2020 },
			rules2020
		);

		const auto hs2021 = make_holiday_schedule(
			std::chrono::year{ 2021 },
			rules
		);

		const auto SpringBankHoliday2 = named_holiday{ std::chrono::June / std::chrono::day{ 2u } };
		const auto PlatinumJubileeHoliday = named_holiday{ std::chrono::June / std::chrono::day{ 3u } };
		const auto StateFuneral = named_holiday{ std::chrono::September / std::chrono::day{ 19u } }; // Bank Holiday for the State Funeral of Queen Elizabeth II

		auto rules2022 = annual_holiday_storage{};
		rules2022.insert(&NewYearsDay);
		rules2022.insert(&GoodFriday);
		rules2022.insert(&EasterMonday);
		rules2022.insert(&EarlyMayBankHoliday);
		rules2022.insert(&SpringBankHoliday2);
		rules2022.insert(&PlatinumJubileeHoliday);
		rules2022.insert(&SummerBankHoliday);
		rules2022.insert(&StateFuneral);
		rules2022.insert(&ChristmasDay);
		rules2022.insert(&BoxingDay);

		const auto hs2022 = make_holiday_schedule(
			std::chrono::year{ 2022 },
			rules2022
		);

		const auto Coronation = named_holiday{ std::chrono::May / std::chrono::day{ 8u } }; // Bank holiday for the coronation of King Charles III

		auto rules2023 = annual_holiday_storage{};
		rules2023.insert(&NewYearsDay);
		rules2023.insert(&GoodFriday);
		rules2023.insert(&EasterMonday);
		rules2023.insert(&EarlyMayBankHoliday);
		rules2023.insert(&Coronation);
		rules2023.insert(&SpringBankHoliday);
		rules2023.insert(&SummerBankHoliday);
		rules2023.insert(&ChristmasDay);
		rules2023.insert(&BoxingDay);

		const auto hs2023 = make_holiday_schedule(
			std::chrono::year{ 2023 },
			rules2023
		);

		const auto hs2024_2025 = make_holiday_schedule(
			{ std::chrono::year{ 2024 }, std::chrono::year{ 2025 } },
			rules
		);

		return hs2018_2019 + hs2020 + hs2021 + hs2022 + hs2023 + hs2024_2025;
	}


	inline auto make_TARGET2_holiday_schedule() -> schedule
	{
		// from https://www.ecb.europa.eu/paym/target/target2/profuse/calendar/html/index.en.html

		const auto LaborDay = named_holiday{ May / 1d }; // should it be in calendar?

		auto rules = annual_holiday_storage{};
		rules.insert(&NewYearsDay);
		rules.insert(&GoodFriday);
		rules.insert(&EasterMonday);
		rules.insert(&LaborDay);
		rules.insert(&ChristmasDay);
		rules.insert(&BoxingDay);

		return make_holiday_schedule(
			{ 2002y, 2023y },
			rules
		);
	}


	inline auto make_SIX_holiday_schedule() -> schedule
	{
		// from https://www.six-group.com/en/products-services/the-swiss-stock-exchange/market-data/news-tools/trading-currency-holiday-calendar.html#/

		const auto BerchtoldDay = offset_holiday<named_holiday>{ NewYearsDay, days{ 1 } };
		const auto LaborDay = named_holiday{ May / 1d }; // should it be in calendar?
		const auto NationalDay = named_holiday{ August / 1d };

		auto rules = annual_holiday_storage{};
		rules.insert(&NewYearsDay);
		rules.insert(&BerchtoldDay);
		rules.insert(&GoodFriday);
		rules.insert(&EasterMonday);
		rules.insert(&LaborDay);
		rules.insert(&AscensionDay);
		rules.insert(&Whitmonday);
		rules.insert(&NationalDay);
//		rules.insert(&ChristmasEve);
		rules.insert(&ChristmasDay);
		rules.insert(&BoxingDay); // should it be called it St. Stephen's Day?
//		rules.insert(&NewYearsEve);

		return make_holiday_schedule(
			{ 1999y, 2024y },
			rules
		);
	}

}
