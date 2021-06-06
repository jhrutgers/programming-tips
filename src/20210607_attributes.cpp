/*
 * Attributes
 *
 * Before, every compiler had its own way of letting the programmer add
 * compiler-specific information to the code. This includes
 * __attribute__((unused)) for unused parameters, or __builtin_expect for the
 * Linux kernel's likely() and unlikely().
 *
 * C++11 generalizes adding this information as attributes. In this example, we
 * will see how you can use it. Execution of this example is not really
 * necessary.
 */

// Attributes are part of the language. These includes are just for this
// example.
#include <cassert>
#include <iostream>


// Wow, look at that!  What is it? It looks strange...
//                  |
//                  |
//                  v
//
//            [[    ﹒    ]]
//
//
// It does not have wings, it makes sudden jumps when you scroll this page down
// (to main()) and back up again -- check out the G-forces of these few photons
// --, it's gone if you look away, and it's friends seem to be embedded
// everywhere... It is an Unusual ASCII-like Phenomena.

static void explain(bool ufo, int count, [[maybe_unused]] char const* bla_bla_what_you_think_it_is)
{
	// The likely attribute (since C++20) indicates that this branch is more
	// likely to be taken.  The compiler may order the produced instructions,
	// such that this path will execute a bit faster, taking cache properties
	// into account, for example. It is like Linux's likely() macro. There is
	// also an [[unlikely]] attribute, but you can guess what that means.

	if(!ufo) [[likely]] {
		std::cout << "I knew it!" << std::endl;
		return;
	}
	// Do not put the [[likely]] in front of the if; it means that it is likely
	// that the if itself is evaluated, regardless of which branch is taken...
	// Put it before the (compound) statement that is likely to be executed.

	std::cout << "The first duty is to the truth." << std::endl;

	switch(count) {
	case 0:
		std::cout << "You did not see any UFO, but still think it was one?" << std::endl;
		break;
	case 1:
		std::cout << "Where are his friends?" << std::endl;
		break;
	// I think, they usually come together...
	[[likely]] case 4:
		std::cout << "There are four lights!" << std::endl;
		[[fallthrough]]; // (C++17) instead of a comment mentioning 'fall-through'
	case 5:
	default:
		std::cout << "Houston, we have a problem." << std::endl;
	}
}

// nodiscard tells that you should use the return value. In this case, it will
// leak otherwise. It exists since C++17.
[[nodiscard]] static auto* take_unclear_picture()
{
	return new char[1701];
}

// Attributes may have arguments, and there can be multiple attributes
// specified within one [[ ... ]]. deprecated is from C++14, noreturn from
// C++11.
[[deprecated("It's a problem anyway."), maybe_unused, noreturn, gnu::section("51")]]
static void halt()
{
	while(true);
}

int main()
{
	// If you have a variable (or function parameter) you don't use, the
	// compiler may warn you about it.

	bool ufo = true;
	assert(ufo); // ufo it not used in release builds.

	// There are many ways to suppress this warning, such as:
	//
	// bool ufo __attribute__((unused)) = true; // gcc
	// Q_UNUSED(ufo); // Qt
	// (void)ufo; // Works for most compilers.
	//
	// In C++11, attributes allow you to give such information to the
	// compiler in single way. Attributes are surrounded by [[ and ]], and
	// can appear almost everywhere in the code. The attributes you can
	// specify, depend on the compiler (unknown attributes to a compiler
	// are just ignored). C++ defines a few common ones. So, this is a
	// better definition:

	[[maybe_unused]] bool uap = true;

	// Again, the [[ ... ]] syntax is around since C++11, but the common
	// attributes are not.  maybe_unused is introduced in C++17. However,
	// gcc allows you use [[gnu::unused]] in C++11/14 as alternative. As
	// you see, attributes can have namespaces, usually related to your
	// compiler.

	explain(ufo, 4, "He has pointy ears too!");
	delete[] take_unclear_picture();

	// Phew, at least that block-like expression thing can be explained
	// easily...
}

/*
 * Further reading:
 *
 * https://en.cppreference.com/w/cpp/language/attributes
 *
 * For example, all of these attributes can be applied for gcc using the
 * attribute syntax [[gnu::...]]:
 * https://gcc.gnu.org/onlinedocs/gcc/Common-Function-Attributes.html#Common-Function-Attributes
 */

