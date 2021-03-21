/*
 * std::tuple
 *
 * An std::tuple is a generalized std::pair; where std::pair is basically a
 * struct that holds two variables (of different types), std::tuple holds any
 * number of variables (of different types). std::tuple is introduced in C++11,
 * as variadic templates were introduced. In C++17, things to got a bit nicer
 * in syntax, so this example uses C++17.
 *
 * Scroll down to main() and follow the execution order. Running the
 * application is not really required.
 */

// This is the header file you need for tuples.
#include <tuple>

// A few other includes for this example.
#include <cmath>
#include <iostream>
#include <utility>

// Just a few types.
struct VVD { int fauteuils; };
struct CDA { int pews; };
struct PvdA { int stools; };
struct D66 { int chairs; };
struct CU { int pews; };
struct GroenLinks { float garden_chairs; };

// Extract the member of a party (struct) using structured binding, regardless
// of the struct type and member name.
template <typename T>
static auto seats(T&& party) {
	auto [ seats ] = party;
	return std::lround(seats);
}

// Compute the sum of seats of all parties within the tuple.
template <typename... Arg>
static auto seats(std::tuple<Arg...>& t)
{
	// C++17's std::apply applies the variables within a tuple as separate
	// arguments to a function. In this case, the lambda is called like:
	// f(std::get<0>(t), std::get<1>(t), ...).
	return std::apply(
		// The function:
		// This is a generic lambda (the argument type is auto), having
		// parties are parameter pack.
		[](auto&&... parties) {
			// This is a fold expression (C++17), which unpacks the
			// parties pack and sums all elements after passing
			// them through seats().
			return (seats(parties) + ...);
		},
		// The arguments to apply to the function:
		t);
}

int main()
{
	// The pre-C++11 way, this creates a 2-tuple (a.k.a. pair).
	std::pair<VVD,CDA> rutteI = std::make_pair(VVD{31}, CDA{21});

	// The values within the pair are accessed using the .first and .second
	// members.
	std::cout << "Rutte I: " <<
		rutteI.first.fauteuils + rutteI.second.pews
		<< " seats" << std::endl;

	// This is equivalent, but using a tuple.
	auto rutteII = std::make_tuple(VVD{41}, PvdA{38});
	// auto is now resolved as std::tuple<VVD,PvdA>.

	// Note that there are no first/second/... members to access the data
	// inside the tuple.  However, there are several ways to get the data
	// out. This is one: std::get by index.
	std::cout << "Rutte II: " <<
		std::get<0>(rutteII).fauteuils + std::get<1>(rutteII).stools
		<< " seats" << std::endl;

	// In case a type is only once in the tuple, std::get can also be used
	// with the type you want to get instead of the index.
	auto rutteIII = std::make_tuple(VVD{33}, CDA{19}, D66{19}, CU{5});
	std::cout << "Rutte III: " <<
		std::get<VVD>(rutteIII).fauteuils + std::get<CDA>(rutteIII).pews +
		std::get<D66>(rutteIII).chairs + std::get<CU>(rutteIII).pews
		<< " seats" << std::endl;

	// Using std::make_tuple saves you from specifying the template
	// arguments explicitly.  Since C++17, the class template arguments can
	// be deducted.
	std::tuple rutteIV_base{VVD{35}, D66{23}, CDA{15}};

	// By using structured bindings, one can easily access or extract the
	// contents of a tuple into separate variables. This also works for
	// arrays and structs.  This syntax is new in C++17. It's C++11
	// equivalent std::tie accomplishes the same, but with not the nice
	// syntax, so forget about ties.
	auto& [ vvd, d66, cda ] = rutteIV_base;

	// In this case, vvd, d66 and cda are variables with the type as
	// dictated by the tuple.  As the type in this example is auto&
	// (instead of auto), they are in fact references to the values within
	// the tuple.
	std::cout << "Rutte IV base: " <<
		vvd.fauteuils + d66.chairs + cda.pews
		<< " seats" << std::endl;


	// Lets see what we can do if we add things to a tuple.
	auto rutteIVa = std::tuple_cat(rutteIV_base, std::make_tuple(GroenLinks{8}));
	// Now, rutteIVa is a std::tuple<VVD,D66,CDA,GroenLinks>.
	std::cout << "Rutte IVa: " << seats(rutteIVa) << " seats" << std::endl;

	auto rutteIVb = std::tuple_cat(rutteIV_base, std::make_tuple(CU{5}));
	std::cout << "Rutte IVb: " << seats(rutteIVb) << " seats" << std::endl;

	// There can be other variations... Oops, I unpacked variations...
	// Oops, again!  This will take a while to evaluate them all; way
	// longer than anyone would like or think...  Oops, although the last
	// unpack may be wise to do, though.

	// Final thought on tuples: tuples are very generic, they don't tell
	// the programmer anything about the application. It is often more
	// intuitive to make it concrete, such as a Coalition class with
	// concrete members or a list with some logical ordering like the party
	// size.  That will make your API more straightforward, although it is
	// a bit more work to write a wrapper class to hold thinks, like a
	// tuple can do for you.
}

/*
 * Further reading:
 *
 * https://en.cppreference.com/w/cpp/utility/tuple
 * https://en.cppreference.com/w/cpp/utility/tuple/tie
 * https://en.cppreference.com/w/cpp/utility/apply
 * https://en.cppreference.com/w/cpp/language/structured_binding
 * https://en.cppreference.com/w/cpp/language/fold
 */

