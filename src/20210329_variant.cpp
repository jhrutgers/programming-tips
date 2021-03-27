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

#include <algorithm>
#include <cstdio>
#include <exception>
#include <iostream>
#include <typeinfo>
#include <utility>

//////////////////////////////////////
// Some type name demangling here
//
// vvv Skip vvv

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

// ^^^ Skip ^^^
//////////////////////////////////////


struct nop {};
struct mov {};
struct jmp {};
struct b {};

struct Intel {
	Intel() = default;
	Intel(mov i) : instr(i) {}
	Intel(jmp i) : instr(i) {}
	std::variant<nop, mov, jmp> instr;
};

struct ARM {
	// Even smaller...
	template <typename I = nop> ARM(I const& i = I()) : instr(i) {}
	std::variant<nop, mov, b> instr;
};

template <typename CPU>
static void run(CPU&& cpu)
{
	std::visit(
		[](auto i) {
			std::cout
				<< name<CPU>() << ": "
				<< name(i) << std::endl;
		},
		cpu.instr);
}


int main()
{
	// First of all, what is wrong with unions?
	union {
		long thumb;
		double core;
	} down = {2L}; // initialize thumb

	static_assert(sizeof(down) == std::max({sizeof(down.thumb), sizeof(down.core)}));

	// Make core the active member.
	down.core = 25.4;
	printf("%g\n", down.core);
//	printf("%ld\n", down.thumb); // Undefined behavior. Leave it commented out...

	// Like a std::tuple.
	std::variant<long, double> up{2L};
	up = 25.4;
	try {
		std::cout << std::get<1>(up) << std::endl;
		std::cout << std::get<0>(up) << std::endl;
	} catch(std::bad_variant_access const&) {
		std::cout << "bad access" << std::endl;
	}

	std::variant<Intel, ARM> cpu0; // Intel
	std::variant<Intel, ARM> cpu1{ARM{}}; // by copy-construct
	std::variant<Intel, ARM> cpu2{Intel{jmp()}}; // by copy-construct
//	std::variant<Intel, ARM> cpu3{mov()}; // converting constructor; don't use (is broken)
	std::variant<Intel, ARM> cpu4(std::in_place_type<ARM>, b()); // in-place construct
	std::variant<std::monostate, Intel, ARM> cpu5;

//	cpu1 = jmp(); // converting assignment; don't use (is broken)
	cpu1 = Intel{jmp()};
	cpu2.emplace<ARM>(b());

	// Visit the active member.
	std::visit([](auto& cpu) { run(cpu); }, cpu0);
	std::visit([](auto& cpu) { run(cpu); }, cpu1);
	std::visit([](auto& cpu) { run(cpu); }, cpu2);
//	std::visit([](auto& cpu) { run(cpu); }, cpu3);
	std::visit([](auto& cpu) { run(cpu); }, cpu4);
//	std::visit(run, cpu4); // won't work, need to specify run's type; use lambda instead.

	// More access methods.
	std::cout << cpu1.index() << std::endl;
	std::cout << std::holds_alternative<Intel>(cpu1) << std::endl;
	run(std::get<1>(cpu4));
	run(std::get<Intel>(cpu1));
	(void)cpu5;
}

