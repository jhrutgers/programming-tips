/*
 * Three-way-comparison operator
 *
 * Things can be compared using the operators == != < > <= >=.  C++20
 * introduces the three-way comparison <=>. Actually, it saves you a lot of
 * work.
 */

// The operator <=> is part of the language. The ordering types below are
// defined in this header.
#include <compare>

// These are just for this example.
#include <array>
#include <cstdint>
#include <iostream>
#include <limits>
#include <string_view>

using std::literals::string_view_literals::operator""sv;

class Gas {
};

class Country {
public:
	double households{};
	double gas_storage_capacity_kWh{};
	double average_usage_kWhpy{std::numeric_limits<double>::quiet_NaN()};

	double max_gas_reserve_per_household() const { return gas_storage_capacity_kWh / households; }

	// We only implement two of them...
	bool operator==(Country const& o) const
	{
		return this == &o || max_gas_reserve_per_household() == o.max_gas_reserve_per_household();
	}

	bool operator<(Country const& o) const
	{
		return max_gas_reserve_per_household() < o.max_gas_reserve_per_household();
	}

	// ...and derive the rest.
	bool operator!=(Country const& o) const { return !(*this == o); }
	bool operator<=(Country const& o) const { return *this < o || *this == o; }

	// Not true when max_gas_reserve_per_household() would return nan, as
	// that would result in a partial ordering (neither object would be
	// less or greater than the other), which this implementation does not
	// take into account.
	bool operator>(Country const& o) const { return !(*this <= o); }
	bool operator>=(Country const& o) const { return !(*this < o); }
};

class Storage {
public:
	std::string_view name;
	double capacity_TWh{};
	double max_withdrawal_rate_GWhpd{};
	double max_injection_rate_GWhpd{};

	std::partial_ordering operator<=>(Storage const& o) const
	{
		auto a = capacity_TWh;
		auto b = o.capacity_TWh;

		return
			a == b ? std::partial_ordering::equivalent :
			a < b  ? std::partial_ordering::less :
			b < a  ? std::partial_ordering::greater :
			         std::partial_ordering::unordered; // such as NaN.

		// return a <=> b; would be easier...
	}
};

class GasImport {
public:
	std::string_view country;
	uint8_t imported_pct{};

	std::strong_ordering operator<=>(GasImport const& o) const
	{
		auto a = imported_pct;
		auto b = o.imported_pct;

		return
			a == b ? std::strong_ordering::equivalent :
			a < b  ? std::strong_ordering::less :
			         std::strong_ordering::greater;

		// return a <=> b; would be easier...
	}
};

class Price {
public:
	double EUR_per_kWh;
	std::string_view country;

	auto operator<=>(Price const& o) const = default;
};

int main()
{
	// Can't compare, as there is no comparison operator defined for Gas.
//	Gas natural;
//	Gas green;
//	std::cout << std::boolalpha << "natural == green ?  " << (natural == green) << std::endl;

	// Designated initializers! Finally!
	Country netherlands{.households = 7.9e6, .gas_storage_capacity_kWh = 144.6e9, .average_usage_kWhpy = 12'104};
	Country belgium{5.0e6, 9.0e9, 23'260};
	Country uk(27.8e6, 17.5e9, 12'000); // Looks strange since '20, huh?

	std::cout << "Max gas reserve per household in The Netherlands is " <<
		netherlands.max_gas_reserve_per_household() << " kWh (average usage is " <<
		netherlands.average_usage_kWhpy << " kWh/y) (assuming nothing is reserved for industry)" << std::endl;

	std::cout << "Belgium has " <<
		belgium.max_gas_reserve_per_household() << " kWh (average usage is " <<
		belgium.average_usage_kWhpy << " kWh/y)" << std::endl;

	std::cout << "Belgium has " << (belgium < netherlands ? "less" : "more") << " then The Netherlands" << std::endl;

	std::cout << "UK has " <<
		uk.max_gas_reserve_per_household() << " kWh (average usage is " <<
		uk.average_usage_kWhpy << " kWh/y)" << std::endl;

	Storage NL0201{"Grijpskerk"sv, 27.67, 719.33, 172.92};
	Storage NL0202{"Norg"sv, 59.34, 791.18, 448.75};
	Storage UK1102{"Stublach"sv, std::numeric_limits<double>::quiet_NaN(), 170.0, 170.0};

	std::cout << (NL0201 <=> NL0202 < 0) << std::endl;
	std::cout << (NL0201 < NL0202) << std::endl;
	std::cout << (NL0201 > NL0202) << std::endl;

	std::cout << (NL0201 <=> UK1102 == 0) << std::endl;
	std::cout << (NL0201 <=> UK1102 < 0) << std::endl;
	std::cout << (NL0201 <=> UK1102 > 0) << std::endl;

	std::array eu_import{
		GasImport{"Norway", 28},
		GasImport{"Russia", 18},
		GasImport{"Belarus", 9},
		GasImport{"Ukraine", 17}};

	std::sort(eu_import.begin(), eu_import.end());
	for(auto const& i : eu_import)
		std::cout << i.country << ": " << +i.imported_pct << " % of total EU import" << std::endl;

	std::array prices{
		Price{0.0498, "Belgium"},
		Price{0.062, "Germany"},
		Price{0.028, "Latvia"},
		Price{0.1073, "Sweden"},
		Price{0.101, "The Netherlands"},
	};

	std::sort(prices.begin(), prices.end(), std::greater<>());

	for(auto const& p : prices)
		std::cout << p.country << ": EUR " << p.EUR_per_kWh << " per kWh" << std::endl;
}

/*
 * Further reading:
 *
 * https://en.cppreference.com/w/cpp/language/operator_comparison#Three-way_comparison
 * https://en.cppreference.com/w/cpp/language/default_comparisons
 */

