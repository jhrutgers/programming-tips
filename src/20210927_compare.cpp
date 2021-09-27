/*
 * Three-way-comparison operator
 *
 * Things can be compared using the operators == != < > <= >=.  C++20
 * introduces the three-way comparison <=>, a.k.a. "spaceship operator".
 * Actually, it saves you a lot of work.
 *
 * Hyperjump down to main() and follow the program flow from there.
 */

// The operator <=> is part of the language. The ordering types below are
// defined in this header.
#include <compare>

// These are just for this example.
#include <algorithm>
#include <array>
#include <cstdint>
#include <iostream>
#include <limits>
#include <string_view>

using namespace std::literals;

// By default, there are no comparison operators defined.  So, you cannot
// compare instances of this class.
class Gas {
};

// The pre-C++20 way: define all relevant operators.
class Country {
public:
	double households{};
	double gas_storage_capacity_kWh{};
	double average_usage_kWhpy{std::numeric_limits<double>::quiet_NaN()};

	double max_gas_reserve_per_household() const { return gas_storage_capacity_kWh / households; }

	// We only implement two of them...
	friend bool operator==(Country const& a, Country const& b)
	{
		return &a == &b || a.max_gas_reserve_per_household() == b.max_gas_reserve_per_household();
	}

	friend bool operator<(Country const& a, Country const& b)
	{
		return a.max_gas_reserve_per_household() < b.max_gas_reserve_per_household();
	}

	// ...and derive the rest.
	friend bool operator!=(Country const& a, Country const& b) { return !(a == b); }
	friend bool operator<=(Country const& a, Country const& b) { return !(b < a); }

	// Not true when max_gas_reserve_per_household() would return nan, as
	// that would result in a partial ordering (neither object would be
	// less or greater than the other), which this implementation does not
	// take into account.
	friend bool operator>(Country const& a, Country const& b) { return b < a; }
	friend bool operator>=(Country const& a, Country const& b) { return !(a < b); }
};

// In C++20, only the <=> would be required, and the compiler will create the
// others for you automatically.
class Storage {
public:
	std::string_view name;
	double capacity_TWh{};
	double max_withdrawal_rate_GWhpd{};
	double max_injection_rate_GWhpd{};

	// Note that the result is std::partial_ordering, std::string_ordering
	// or std::weak_ordering.  In this case, we use the partial ordering.
	// This means that two object may have an order (one is before the
	// other), but some values may not have such order.  For example, a
	// float's nan has no other to other values. However, an int has a
	// strong ordering. Weak ordering is for example when comparing string
	// without case sensitivity; two string may be different, but compare
	// the same.
	std::partial_ordering operator<=>(Storage const& o) const
	{
		auto a = capacity_TWh;
		auto b = o.capacity_TWh;

		return
			a == b ? std::partial_ordering::equivalent :
			a < b  ? std::partial_ordering::less :
			b < a  ? std::partial_ordering::greater :
			         std::partial_ordering::unordered; // such as NaN.

		// It would be easier to just do this:
		//
		// return a <=> b;
		//
		// ...but the implementation above is a bit more explicit for
		// this example.
	}

	// Great, only one comparison function. These friends as of Country
	// above are great, especially when there are always there and you can
	// rely on them.
};

// Similar to Storage, but now using std::string_ordered.
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

// It even gets easier. If you specify that you want a default <=> operator,
// the compiler will do everything for you. Basically, it iterates over all
// member variables in the class, and compares them using <=>. The first
// non-equal result is returned.
class Price {
public:
	double EUR_per_kWh;
	std::string_view country;

	auto operator<=>(Price const& o) const = default;

#if 0
	// Now, the compiler generates a comparison like the following:
	std::partial_ordering operator<=>(Price const& o) const
	{
		if(auto c = EUR_per_kWh <=> o.EUR_per_kWh; c != 0)
			return c;

		if(auto c = country <=> o.country; c != 0)
			return c;

		return std::partial_ordering::equivalent;
	}

	// This only works if you can compare all members variables in this
	// order.  This wouldn't have worked for Storage or GasImport, as they
	// would order them based on the country name.
#endif
};

int main()
{
	// Comparing primitive types (int, float, etc.) is always possible, as
	// there exist default comparison operators for these types. If you
	// have your own class, you should define them yourself.

#if 0
	// Because, if you don't, you can't compare. In this case, there is no
	// comparison operator defined for Gas.
	Gas natural;
	Gas green;
	std::cout << std::boolalpha << "natural == green ?  " << (natural == green) << std::endl;
#endif

	// Country does define the operators. Although you could hide all kinds
	// of counter-intuitive behavior, assume that the are implemented
	// according to what you expect. In this case, countries are compared
	// based on their potential gas reserve.

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

	std::cout << "UK has " <<
		uk.max_gas_reserve_per_household() << " kWh (average usage is " <<
		uk.average_usage_kWhpy << " kWh/y)" << std::endl;

	// Comparing works fine.
	std::cout << "Belgium has " << (belgium < netherlands ? "less" : "more") << " than The Netherlands" << std::endl;

	std::cout << "The smallest gas reserve from these three countries is only " <<
		std::min({netherlands, belgium, uk}).max_gas_reserve_per_household() << " kWh" << std::endl << std::endl;
	// Likely, the country with the smallest reserve can rely on EU's
	// reserves, as we are all friends, right?


	// But define these operators is a bit cumbersome. Since C++20, there
	// is an alternative using <=>.  The idea of a <=> b is the same as
	// memcmp(a, b) or strcmp(a, b):
	//
	// - the result compares equal to 0 when a and b are equal
	// - the result compares < 0 when a < b
	// - the result compares > 0 when a > b
	//
	// Note that memcmp() and strcmp() actually return an int, while <=>
	// returns some object that should be compared to 0.
	//
	// Great benefit is that when you define only the <=> operator, all
	// others are defined automatically for you.
	//
	// Check out what happens:

	Storage NL0201{"Grijpskerk"sv, 27.67, 719.33, 172.92};
	Storage NL0202{"Norg"sv, 59.34, 791.18, 448.75};
	Storage UK1102{"Stublach"sv, std::numeric_limits<double>::quiet_NaN(), 170.0, 170.0};

	std::cout << (NL0201 <=> NL0202 < 0) << std::endl;
	std::cout << (NL0201 < NL0202) << std::endl;
	std::cout << (NL0201 > NL0202) << std::endl << std::endl;

	std::cout << (NL0201 <=> UK1102 == 0) << std::endl;
	std::cout << (NL0201 <=> UK1102 < 0) << std::endl;
	std::cout << (NL0201 <=> UK1102 > 0) << std::endl << std::endl;

	// Directly using <=> does not really make sense this way often, but it
	// makes implementing comparison in the API easier.


	// Once you have this operator in place, you can use all default C++
	// tricks that rely on them, such as std::sort.

	std::array eu_import{
		GasImport{"Norway", 28},
		GasImport{"Russia", 18},
		GasImport{"Belarus", 9},
		GasImport{"Ukraine", 17}};

	std::sort(eu_import.begin(), eu_import.end());
	for(auto const& i : eu_import)
		std::cout << i.country << ": " << +i.imported_pct << " % of total EU import" << std::endl;

	std::cout << std::endl;


	// Again, but now with auto-generated comparison functions.

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
