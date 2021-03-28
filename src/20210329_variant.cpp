/*
 * std::variant
 *
 * The members of a unions overlap in memory, so only one member is 'the
 * active' one.  However, the compiler cannot determine which one is active.
 * So, it is up to the programmer to access only the correct member, otherwise
 * it is undefined behavior -- anything could happen, such as the instantaneous
 * dissolving of your CPU.
 *
 * C++17 adds std::variant, which is a type-safe version of a union, which does
 * track the active member, properly destructs the previous when switching the
 * active member, and throws exceptions when accessed wrong.
 *
 * Scroll down to main() and follow the execution flow.
 */

// This is the header you need for std::variant.
#include <variant>

// A few headers for this example.
#include <cstdio>
#include <exception>
#include <iostream>
#include <typeinfo>
#include <utility>

struct nop {};
struct mov {};
struct jmp {};
struct b {};
struct stm {};

struct Intel {
	Intel() = default;
	Intel(nop i) : instr(i) {}
	Intel(mov i) : instr(i) {}
	Intel(jmp i) : instr(i) {}

	// This struct holds one simple instr.
	std::variant<nop, mov, jmp> instr;
};

struct ARM {
	// This is an even smaller definition...
#if __cplusplus < 202002L
	template <typename I = nop> ARM(I const& i = I()) : instr(i) {}
#endif
	std::variant<nop, mov, b, stm> instr;
};


// Forward-declare of some dirty type name (demangling) functions.
template <typename T> static std::string name();
template <typename T> static std::string name(T const& x);

// A function that indicates what the current instr is for a given CPU.
template <typename CPU>
static void run(CPU&& cpu)
{
	std::visit(
		[](auto&& i) {
			std::cout
				<< name<CPU>() << ": "
				<< name(i) << std::endl;
		},
		cpu.instr);
}

// Function specialization for a non-CPU.
template <>
void run<std::monostate&>(std::monostate& cpu)
{
	std::cout << "nothing running" << std::endl;
}

int main()
{
	// First of all, what is wrong with good old unions?  Members in a
	// union share the same memory. There can only be one active member (as
	// already said, that must be tracked by the programmer). There is a
	// limitation on data types in a union: they cannot have custom
	// copy/move constructors/assignment or destructor.
	union {
		// These two variables are unrelated.
		long thumb;
		double core;
	} down = {2L}; // The initializer sets the first member.
	// So, thumb is now the active member.

	// The size of the union can be predicted -- the largest member must
	// fit -- but the offset/padding/alignment of smaller members is
	// implementation-defined (so, check your compiler manual).
	static_assert(sizeof(down) == std::max(sizeof(down.thumb), sizeof(down.core)));

	// This would make core the active member:
	down.core = 25.4;
	// This is OK:
	printf("union's double: %g\n", down.core);
	// But this is undefined behavior. Leave it commented out!
//	printf("union's long: %ld\n", down.thumb);


	// C++17 has a type-safe variant of unions: std::variant.
	// The template parameters are the 'members' within the 'union'.
	// So, this is equivalent to union down above.
	std::variant<long, double> up{2L};

	// You cannot access the members directly, like you can with a union,
	// but it depends on the type that is used. In the initializer above,
	// the long type is saved, as the variant is initialized with a long.
	// In the assignment below, the double becomes the 'active field', as
	// a double is assigned to the tuple.
	up = 25.4;

	try {
		// Access of the member can be done by type...
		std::cout << "variant's double: " << std::get<double>(up) << std::endl;
		// ...or by index of template parameters.
		std::cout << "variant's long: " << std::get<0>(up) << std::endl;
		// As variant held a double value, and not a long, the latter
		// std::get is invalid.  It is, however, properly defined
		// behavior: it will throw an std::bad_variant_exception.
	} catch(std::bad_variant_access const&) {
		std::cout << "Pfew, just saved my CPU..." << std::endl;
	}

	// Let's have a look what the variant API gives us.

	// Default initialized: this will default-construct the first type (Intel).
	std::variant<Intel, ARM> cpu0;

	// Intermezzo: std::visit calls the first parameter (a Callable thing)
	// for the held value in the variant. In this case, the lambda is
	// called with (some wrapper around) Intel as argument. As you don't
	// know upfront whether the argument will be an Intel or ARM, use a
	// generic lambda (a lambda with auto as argument type).
	std::visit([](auto&& cpu) { std::cout << "cpu0: "; run(cpu); }, cpu0);

	// Copy-construct one of the variant's types.
	std::variant<Intel, ARM> cpu1{ARM{}};
	std::variant<Intel, ARM, ARM> cpu2{Intel{jmp()}};

	std::visit([](auto&& cpu) { std::cout << "cpu1: "; run(cpu); }, cpu1);
	std::visit([](auto&& cpu) { std::cout << "cpu2: "; run(cpu); }, cpu2);

	// In-place construct (so, without any copy/move). First argument is
	// the type to construct wrapped in an std::in_place_type. All
	// following arguments are passed to the constructor of ARM in this
	// case.
	std::variant<Intel, ARM> cpu3{std::in_place_type<ARM>, b()};
	std::visit([](auto&& cpu) { std::cout << "cpu3: "; run(cpu); }, cpu3);

	// When an argument is supplied that can be converted to one of the
	// types in the variant, in this case b can be passed through ARM's
	// constructor, an ARM value will be held by the variant.  However, the
	// converting constructor has some issues. A bug in C++17 makes it
	// unreliable, or even plain broken (someone probably executed the
	// printf() line before while writing the standard).
	//
	// Moreover, even if it would work properly, it is still questionable
	// what would be saved in the variant, as the conversion path taken
	// (and therefore which value is stored) is not reported while
	// compiling, you could be tricked.
	//
	// So, don't use the converting constructor. Only initialize explicitly
	// the correct type.
//	std::variant<Intel, ARM> cpu4{b()};
//	std::visit([](auto&& cpu) { std::cout << "cpu4: "; run(cpu); }, cpu4);

	// A variant cannot be empty*. If you need conceptually an optional
	// variant, use std::monostate as first type. This is literally defined
	// as 'struct monostate {};'. So, the variant won't be really empty,
	// but you know when the dummy type is used. You could use your own
	// type to indicate the 'empty' state.
	//
	// * There are a few rare corner case, though, when exceptions are thrown
	//   during specific operations. But ignore that for now.
	std::variant<std::monostate, Intel, ARM> cpu5;
	std::visit([](auto&& cpu) { std::cout << "cpu5: "; run(cpu); }, cpu5);

	// Having a variant, you can update the current (active) value, or
	// switch between types.  By doing so, std::variant takes care of
	// destruction/construction when switching types.  So, the limitation
	// of union does not apply to a variant.
	cpu5 = Intel{jmp()};
	std::visit([](auto&& cpu) { std::cout << "cpu5 assigned: "; run(cpu); }, cpu5);

	// Assigning will do a copy or move into the variant. You can also
	// construct in-place, by using emplace() instead:
	cpu5.emplace<ARM>(stm());
	std::visit([](auto&& cpu) { std::cout << "cpu5 emplaced: "; run(cpu); }, cpu5);

	// As with the converting constructor, there is also a converting
	// assignment.  However, it suffers from the same issues as the
	// constructor, so don't use.  Always be explicit which type you need.
//	cpu5 = jmp();

	// To determine what is currently in the variant, you could just try
	// and catch the exception.  There are other ways to check.
	if(std::holds_alternative<ARM>(cpu5)) {
		std::cout << "Yes! Got an nVidia! Well... Err... Confused." << std::endl;
		run(std::get<ARM>(cpu5));
	}

	// By index is not as nice as by type, as you need to know the order of
	// the types. So, prefer the other one.
	if(cpu5.index() == 2) {
		std::cout << "Yes! At least I got something at index " << cpu5.index() << std::endl;
		run(std::get<2>(cpu5));
	}
}

/*
 * Further reading:
 *
 * https://en.cppreference.com/w/cpp/utility/variant
 *
 * Software has bugs, even the standard.
 * This is what I encountered during preparation of this tip:
 *
 * - Why needs the ARM struct in this example a constructor?
 *   https://stackoverflow.com/questions/43407814/why-can-an-aggreggate-struct-be-brace-initialized-but-not-emplaced-using-the-sa
 * - What is wrong with std::variant's converting constructor/assignment?
 *   http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2018/p0608r3.html
 * - Why can't run() in the form of an Abbreviated function template (C++20) be
 *   passed argument instead of the lambda for std::visit? They seem
 *   equivalent... Related?
 *   https://gcc.gnu.org/bugzilla/show_bug.cgi?id=64194
 */







//////////////////////////////////////
// Some type name demangling here
//
// vvv Ignore this part vvv

#if defined(__GNUC__) || defined(__clang__)
#  include <cxxabi.h>
#endif

template <typename T>
static std::string name()
{
#ifdef __cpp_rtti
#  if defined(__GNUC__) || defined(__clang__)
	int status = 1;
	char* demangled = abi::__cxa_demangle(typeid(T).name(), nullptr, nullptr, &status);
	std::string res = demangled;
	free(demangled);
	if(status)
		throw std::runtime_error("Cannot demangle");
	return res;
#  else
	return typeid(T).name();
#  endif
#else
#  error "Need RTTI for this example"
	return "?";
#endif
}

template <typename T>
static std::string name(T const& x)
{
	return name<T>();
}

// ^^^ Ignore this part ^^^
//////////////////////////////////////

