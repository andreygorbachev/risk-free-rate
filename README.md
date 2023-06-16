# risk-free-rate

Resets are expected to be setup once and read many times.


[1] https://www.bankofengland.co.uk/markets/sonia-benchmark

[2] https://www.newyorkfed.org/markets/reference-rates/sofr

[3] https://www.ecb.europa.eu/stats/financial_markets_and_interest_rates/euro_short-term_rate/html/index.en.html

[4] https://www.six-group.com/en/products-services/the-swiss-stock-exchange/market-data/indices/swiss-reference-rates.html

[5] https://www.bankofcanada.ca/rates/interest-rates/corra/

[6] https://www.boj.or.jp/en/statistics/market/short/mutan/index.htm


The format of the inputs should match the real world, so if SONIA is quoted as 3.3333 then this is how it should be entered (as a percentage, but internally for calculation purposes it would be converted to 0.033333). If SONIA Compound Index is quoted as 103.33333333, then this is how it should be entered.
