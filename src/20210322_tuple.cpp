/*
 * std::tuple
 *
 * An std::tuple is a generalized std::pair; where std::pair holds two
 * variables (of different types), std::tuple holds any number of variables (of
 * different types). std::tuple is introduced in C++11, as variadic templates
 * were introduced.
 *
 * Scroll down to main() and follow the execution order.
 */

#include <tuple>

#include <utility>
#include <iostream>

struct VVD { int fauteuils; };
struct CDA { int pews; };
struct PvdA { int stools; };
struct D66 { int chairs; };
struct CU { int pews; };
struct GroenLinks { int garden_chairs; };

int main()
{
	// The pre-C++11 way
	std::pair<VVD, CDA> rutteI = std::make_pair(VVD{31}, CDA{21});
	std::cout << "Rutte I: " << rutteI.first.fauteuils + rutteI.second.pews << " seats" << std::endl;

	// Equivalent, but using a tuple. Access by index.
	auto rutteII = std::make_tuple(VVD{41}, PvdA{38});
	std::cout << "Rutte II: " << std::get<0>(rutteII).fauteuils + std::get<1>(rutteII).stools << " seats" << std::endl;

	// Access by type (if it's unique).
	auto rutteIII = std::make_tuple(VVD{33}, CDA{19}, D66{19}, CU{5});
	std::cout << "Rutte III: " <<
		std::get<VVD>(rutteIII).fauteuils + std::get<CDA>(rutteIII).pews +
		std::get<D66>(rutteIII).chairs + std::get<CU>(rutteIII).pews
		<< " seats" << std::endl;

	// Class template argument deduction (since C++17).
	std::tuple rutteIV{VVD{35}, D66{23}, CDA{15}, CU{5}, GroenLinks{8}};

	// Structured bindings (since C++17, which is easier than std::tie of C++11).
	auto& [ vvd, d66, cda, cu, groenlinks ] = rutteIV;

	std::cout << "Rutte IV? "  << vvd.fauteuils + d66.chairs + cda.pews + cu.pews + groenlinks.garden_chairs << " seats" << std::endl;

	{
		int c = 0;
		std::swap(c, std::get<CU>(rutteIV).pews);
		std::cout << "Rutte IVa? " << vvd.fauteuils + d66.chairs + cda.pews + cu.pews + groenlinks.garden_chairs << " seats" << std::endl;
		std::swap(c, std::get<CU>(rutteIV).pews);
	}

	{
		int c = 0;
		std::swap(c, groenlinks.garden_chairs);
		std::cout << "Rutte IVb? " << vvd.fauteuils + d66.chairs + cda.pews + cu.pews + groenlinks.garden_chairs << " seats" << std::endl;
		std::swap(c, groenlinks.garden_chairs);
	}
}

/*
 * Further reading:
 *
 * https://en.cppreference.com/w/cpp/utility/tuple
 * https://en.cppreference.com/w/cpp/utility/tuple/tie
 * https://en.cppreference.com/w/cpp/language/structured_binding
 */

