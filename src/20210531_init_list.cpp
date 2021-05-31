/*
 * std::initializer_list
 *
 * C++11 introduces the initializer list, which allows a convenient way of
 * passing multiple values to functions, just like array initialization.
 *
 * Let's see how that works. Scroll down to main() and follow the execution
 * flow from there.
 */

// This header defines std::initializer_list. The others are just for this
// example.
#include <initializer_list>

#include <iostream>
#include <limits>
#include <tuple>
#include <typeinfo>
#include <vector>

// Without std::initializer_list, you could accept all arguments using a
// parameter pack.  Every element can be of a different type.
template <typename... T>
static void earth_surface_fraction(T&&... args)
{
	std::cout << "Surface [fraction of Earth]: ";

	// Handling the pack is a bit tricky, though.
	std::tuple<T...> const xs{args...};

	// C++17's fold expression with std::apply. See also 20210405_pack.
	std::apply([](auto&&... x){
		((std::cout << std::fixed << static_cast<double>(x) / 510.1e6 << " "), ...); },
		xs);

	// Using C++14, you could create an integer sequence and apply it to
	// std::get, but that is even harder.

	std::cout << std::endl;
}

// This function accepts an std::initializer_list. The list has a template
// parameter, that specifies which type is contained, just like an std::array.
// All elements have the same type.
//
// std::initializer_list is a light-weight object. You should pass (and accept)
// it by value.
//
// This is a normal function, but you can also create a (non-explicit)
// constructor that accepts the initializer_list.  This is what
// std::array/vector/list/... do.
template <typename T>
double earth_ratio_to_plant_forest_to_consume_CO2eq_production(std::initializer_list<T> l)
{
	double total = 0;

	// You can iterate over it, and ask for its .size(). It does not have
	// any other member functions. And you cannot add or remove elements to
	// or from it. So, you usually pass it to an std::vector, for example,
	// or iterate it like this:
	for(auto const& x : l) {
		total +=
			static_cast<double>(x)   // one year CO2eq in Gt
			/ ( 510.1e12             // Earth surface in m2
			  * 0.6177               // absorb kg CO2/m2 forest/year
			  * 1e-12 );             // kg to Gt
	}

	return total;
}

// The universal reference accepts everything, accept the brace-init list...
template <typename T>
static void judge(T&& x)
{
	std::cout << typeid(T).name() << std::endl;
}

int main()
{
	// You already know the old array initialization. Note the braces with
	// values. This will make the array containing five elements. As you
	// also know, the same kind of notation can also be used for struct
	// initialization.
	double shell_revenue[]            = {233.59e9, 308.18e9, 388.38e9, 344.88e9, 180.54e9};
	//                                      2016 ...                           ... 2020
	(void)shell_revenue; // Suppress, don't let anyone bark on this.

	// This definition is equivalent, but with an initializer list. As
	// shell_profit is an object and not an array, the grammar is a bit
	// different under the hood, but it looks the same.
	std::array<double,5> shell_profit = {   4.6e9,   13.0e9,   23.4e9,   15.8e9,  -21.6e9};
	(void)shell_profit;

	// Even nicer; C++17 allows to auto-deduct template arguments (via a
	// deduction guide). So, you can omit the <double,5> here.
	std::array shell_investments      = {  22.1e9,   20.8e9,   23.0e9,   23.0e9,   16.6e9};
	(void)shell_investments;

	// All STL container types (vector/list/map/...) support initializer
	// lists.
	std::vector<double> new_energy    = {   0.2e9,    1e9,      2e9,      2e9,      2e9};

	// The {...} thing defines an std::initializer_list, which is a
	// light-weight wrapper for its arguments, which all must be of the
	// same type.  Its intended use is to be temporary and hold the values
	// that are typically passed to an object for construction, as shown
	// above.
	//
	// The example above constructs an std::initializer_list and creates
	// the vector by assignment.  You can also call the constructor
	// explicitly, like this:
	std::vector global_annual_GtCO2eq_emission(
		{50.0, 50.7, 51.9, 52.4, std::numeric_limits<double>::quiet_NaN()});

	// This way, you can pass any number of elements to the constructor (or
	// any other function that takes an std::initializer_list). Without an
	// initializer_list, you could build such a functionality using
	// templates.  For example:
	earth_surface_fraction(
		41.543f, // The Netherlands
		9.834e6, // US
		30.37e6L // Africa
	);

	// Depending on the context, you may find it easier to pass an
	// initializer list instead.  You have to add the curly braces, though:
	std::cout
		<< "Shell scope 3 emission compensation forest (2016-2019) [number of Earths]: "
		<< earth_ratio_to_plant_forest_to_consume_CO2eq_production(
			{1.545, 1.591, 1.637, 1.551 /*, std::numeric_limits<double>::quiet_NaN()*/ })
		<< std::endl;

	// Type deduction on template parameters is not possible with an
	// brace-init list as a template type. So, this function accepts
	// everything...
	judge(shell_investments);

	// ...except for {...}. You could make an overload of the judge() that
	// accepts an std::initializer_list explicitly, if you need to.
//	judge({2019, 2030 * 0.2, 2035 * 0.45, 2050 * 0}); // does not work

	// When the type is auto, this will create the appropriate
	// std::initializer_list automatically.
	auto verdict = {2019.0, 2030 * 0.45};
	// As the type is known now, it can be passed to judge().
	judge(verdict);

	// Summarized, using std::initializer_list is great, as it expresses
	// the list-like initialization and passing values of a collection.  It
	// is not a replacement for other container types, though.
}

/*
 * Further reading
 *
 * https://en.cppreference.com/w/cpp/utility/initializer_list
 *
 * See also tip 20210412_brace_init
 */
