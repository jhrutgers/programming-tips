/*
 * Structured binding
 *
 * In C++17, a syntax is defined that allows you to access and extract members
 * of arrays, tuples and structs. This syntax is known as Structured binding.
 * Let's checkout how it works.
 *
 * Scroll down to main() and start reading from there.
 */

// Structured binding is part of the language. No includes are required.  These
// are just for this example.
#include <exception>
#include <iostream>
#include <tuple>
#include <type_traits>

struct Module {
	int launch;
	char const* by = "rocket";
};

struct Tiange : Module {};
struct Wentian : Module {};
struct Mengtian : Module {};

using Tiangong = std::tuple<Tiange, Wentian, Mengtian>;

class ISS {
protected:
	using Connection = std::pair<Module&,Module&>;

	Module zarya{1998, "Proton-K"};
	Module unity{1998, "Endeavour"};
	Connection c1{zarya, unity};
	Module destiny{2001, "Atlantis"};
	Connection c2{unity, destiny};
	Module harmony{2007, "Discovery"};
	Connection c3{destiny, harmony};
	Module tranquility{2010, "Endeavour"};
	Connection c4{unity, tranquility};
	Module beam{2016, "Falcon-9"};
	Connection c5{tranquility, beam};

public:
	// Allow access to the 'tuple' elements, as part of being tuple-like.
	// In this case we use SFINAE to make sure that get() is only called
	// with an i that exists. The const qualifier and noexcept specifier
	// are not required, it is just chosen for this example. The only thing
	// you have to do is to make sure that get<i>() is valid for all
	// element indexes in the 'tuple'.
	template <size_t i>
	std::enable_if_t<i < 6u, Module const&> get() const noexcept {
		// i is a template parameter, so this switch is actually
		// constant and (usually) evaluated at compile time. So, there
		// is no overhead from this function.
		switch(i) {
		case 0: return zarya;
		case 1: return unity;
		case 2: return destiny;
		case 3: return harmony;
		case 4: return tranquility;
		case 5: return beam;
		// Well, the default will never be reached, but still feels
		// safe to add anyway.
		default: std::terminate();
		}
	}
};

// These two definitions are required to make ISS tuple-like.
template <>
struct std::tuple_size<ISS> { enum { value = 6 }; };

template <size_t i>
struct std::tuple_element<i, ISS> { using type = Module; };

int main()
{
	// Suppose you have the following array:
	int tiangong_$[]{2011, 2016};
	// Oh, don't give up yet on the syntax. Let's reverse engineer what is
	// in this expression.  It is an array (note the [], with unspecified
	// length) of ints, with the name tiangong_$ (the $ is just a normal
	// character, just like c), and it is brace-initialized with two
	// values, which also specifies the length of the array.

	// Obviously, you can access the array elements like tiangong_$[0] and
	// tiangong_$[1].  However, you can also bind all elements of the array
	// to newly declared variables like this:
	auto [ tg1, tg2 ] = tiangong_$;
	// The [ ... ] denote the structured binding. The type must be auto (or
	// something like auto&, auto volatile, etc.), even though all types
	// will be int, and the number of variables within [ ... ] must match
	// the length of the array. In this example, two variables are created,
	// int tg1 and int tg2, which hold a copy of the values of the array.

	// So, this won't influence tg1.
	tiangong_$[0] = 2018;
	std::cout << "tg1 from " << tg1 << " to " << tiangong_$[0] << std::endl;



	// Structured binding can also be applied to tuples.
	Tiangong tiangong{{2021, "Long March 5B"}, {2022}, {2022}};
	//        Hmm, unlikely? Copy-paste error?  ^^^^ vs ^^^^
	// Here, tiangong is bound to three new variables, in this case
	// references to the elements of the tuple. The name may be a bit
	// deceptive. The 'structured binding' is actually a decomposition, as
	// the whole is taken apart to create separate variables...
	auto& [ t, w, m ] = tiangong;
	// I've seen such a configuration before... Looks familiar... Did
	// someone copy this from somewhere?

	// As m is a Mengtian&, changing it will affect tiangong.
	m.launch = 2023;

	std::cout << "(Planned/expected) launches: "
		<< std::get<0>(tiangong).launch << ", "
		<< w.launch << ", "
		<< std::get<Mengtian>(tiangong).launch << std::endl;

	// In the lines above, it is clear what a big advantage is of
	// structured binding when accessing tuple elements...



	// Even structs (or classes) can be used.
	auto const& [ launched, vehicle ] = t;
	std::cout << "Launched Tiange in " << launched << " by " << vehicle << std::endl;
	// In this case, all members of the t are bound to variables. The order
	// of the variables is the order of the members of the struct. In this
	// case, only the base class Module has members. When the subclass
	// Tiange would also have members, this does not work anymore.  So, it
	// is quite limited what is supported when it comes to inheritance.
	// Only use this when the types are simple.



	// In fact, all tuple-like objects can be used. So, besides std::tuple,
	// you can also decompose std::array and std::pair in the same way.
	// Well, you can make your own type be tuple-like. Consider a complex
	// class, such as ISS. Sneak peek into the internals of ISS above.
	ISS iss{};
	// It has 11 members. In this case, only the Module instances are
	// exposed for structured binding:
	auto const& [ m1, m2, m3, m4, m5, m6 ] = iss;
	std::cout << "First ISS module launched in " << m1.launch << " by " << m1.by << std::endl;
	// To accomplish this:
	// - overload std::tuple_size for your class to indicate the number of
	//   elements;
	// - overload std::tuple_element type to indicate the type of each
	//   element; and
	// - implement a member function get<i>(), that has the element index
	//   as template parameter, and should return the indicated tuple
	//   element.

	// Just a note, only use this tuple-delusion to hide the actual
	// internals from people. It may make the interface easier (when the
	// concept of the class at hand intuitively resembles a tuple), at the
	// cost of a bit more work at the implementation of the API.
}

/*
 * Further reading:
 *
 * https://en.cppreference.com/w/cpp/language/structured_binding
 * https://en.cppreference.com/w/cpp/utility/tuple_size
 * https://en.cppreference.com/w/cpp/utility/tuple_element
 */
