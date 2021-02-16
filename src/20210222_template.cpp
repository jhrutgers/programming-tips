/*
 * Templates
 *
 * You have seen templates when using std::set<std::string>, for example.
 * This tailors the given std::set instance to contain std::strings. This is
 * great, as regardless which type of values the set contains, the logic of
 * handling a set is the same for all of them. So, using templates reduces code
 * duplication.
 *
 * Without templates, you could write a set that contains void*, for example.
 * When using such a set, you have to (allocate and) cast values from/to void*,
 * without the compiler helping you. So, templates allow the compiler to check
 * the types for you, and simplifying interfaces, and therefore reducing bugs.
 *
 * Templates can do more. It is a Turing-complete programming language.
 * However, it quickly becomes too complex to read and understand.
 *
 * Templates are sort of macros at compile time. Template classes and functions
 * are expanded to concrete implementations. Therefore, the performance is as
 * good as any other (non-template) class/function. Well, as templates must be
 * put in a header file -- the compiler has to know the full template code in
 * order to expand it where you want to use it -- it might be even faster than
 * a class/function split in .h/.cpp files, as it can be fully inlined and
 * optimized. Moreover, you can achieve all kinds of complex class inheritance
 * without virtual functions, dynamic_cast, and RTTI.
 *
 * Enough reasons to have a look into them, but let's not go too deep into all
 * the possible complexities. Work you way down through this file. There is no
 * need to compile and run it; the program does not print anything.
 */

// Templates are part of the language. You don't have to include anything
// specifically for them.

#include <string>
#include <vector>
#include <utility>



// These are just different types. There is nothing in these structs that can
// be used, but every type in C++ is different, regardless of their (missing)
// members. These are concrete types, no templates involved.
struct ChAdOx1 {};
struct Ad26 {};
struct Ad5 {};





// Just another type. Note, the only difference between struct and class is
// that by default members of a struct are public, and of a class are private.
// Besides that, they are identical in all aspects. Still no templates...
struct mRNA {
	void deliver() { /* ... */ }
	std::string sequence = u8"GAGAAΨAAAC..."; // special sequence
};

// There you go! Templates! This says: we have a struct named Vector, but it
// takes an arbitrary type as (template) parameter. This template parameter is
// named Virus. Within the implementation of this struct, if the type Virus is
// used, the template parameter is referred to. So, we know how a Vector works,
// regardless of the specific Virus.
//
// You could also write 'template <class Virus>' instead of 'typename', but
// that is equivalent.
template <typename Virus>
struct Vector {
	void deliver() { /* ... */ }
	std::vector<std::string> payload { "ATGTTC", "GTGTTCCTGG", "..." };
private:
	// You can use the template parameter in every way any other type
	// could be used.
	Virus virus;
};

// Let's alias a few types, just to reduce typing...
using Oxford = Vector<ChAdOx1>;
using JohnsonJohnson = Vector<Ad26>;
typedef Vector<std::pair<Ad26,Ad5>> SputnikV;
// C++11 introduced using. This is often preferred above the old typedef, as it
// reads easier and has a few minor other differences, which we just skip in
// this discussion. Consider them to do the same.





// This another class with template parameter. In this case, it defaults to
// void when not specified.  So, when using Vaccin<> as type, it implies using
// Vaccin<void>.
template <typename T = void>
class Vaccin {
public:
	// It is often required to request what the template parameter was for
	// a specific instance, but there no method to get it; Vaccin<int>::T
	// (expecting that T would be int) or something does not exist.  If you
	// want that, alias the type like this. Now, you can retrieve the
	// template parameter like Vaccin<int>::type.
	using type = T;

	// Note that this function assumes that T is like mRNA or Vector, as
	// these types implement the deliver() method. However, if you would
	// instantiate Vaccin<int>, the compiler will complain about int not
	// having a method deliver(). But that only happens when you try to
	// instantiate it, not when including the header file that contains
	// this class definition.
	//
	// If you want to make sure that Vaccin can only be used with specific
	// types, such that you prevent cryptic compiler errors when you
	// provide a non-compatible type, you can do some template magic (or
	// use concepts from C++20), but let's not discuss those, for now.
	void work() { type().deliver(); }
};

// This is very neat: you can provide a specific implementation for the whole
// Vaccin class, in case of specific (properties) of T; explicit template
// specialization. In this case, we provide an implementation for Vaccin<void>,
// that does effectively nothing when work() is called. It is like function
// overloading; the compiler picks the implementation that fits the best*.
//
// * What the 'best' is, is quite complex, but assume that the spec is
//   intuitive**.
//
// ** The classification 'intuitive' in combination with templates is not
//    something that you would encounter often.
//
// Note the syntax.
template <> // We are still doing something when templates.
class Vaccin<void> { // In this case specializing Vaccin<void>.
public:
	// The implementation does not have to be anything like the generic
	// Vaccin class.  It might have the same interface, it might be
	// completely different. It is not something like OO inheritance;
	// Vaccin<void> is just completely different from Vaccin<T>.
	//
	// Often, you would provide the same API; don't make it too hard for
	// the user...
	using type = void;
	void work() {}
};






// Using template specialization, you can do things like type traits.  Consider
// this struct, which the intention to detect if a given Vaccin is based on
// Vector or not. Read is_vector<Vaccin<some_type>>::value to check if it is.
template <typename T>
struct is_vector {};
// The default implementation is empty, so is_vector<float>::value is a compile
// error.

// Now, do a template specialization for all Vaccins. By default, set value to
// false.
template <typename T>
struct is_vector<Vaccin<T>> { enum { value = 0 }; };
// As value is an enum, it won't use any memory. There is no run-time overhead
// in querying is_vector's value, as it is known at compile time. This is what
// makes templates really powerful.

// Another specialization, specifically for Vector Vaccins, regardless of the
// type of Vector.
template <typename T>
struct is_vector<Vaccin<Vector<T>>> { enum { value = 1 }; };
// In main(), we will use this type trait.




// Templates also work for functions. This is even nicer, as you don't have to
// specify the template type when you use the function (but you could do that,
// if you want). The type is auto-deducted from the context. Check main() for
// how it works.
template <typename T>
static void inject(Vaccin<T>& vaccin)
{
	vaccin.work();
}

// You can also have overloaded functions (= same name, different parameters).
// In this case, any type can be passed to inject(). However, the compiler
// picks the best* one.
//
// Note the rvalue-reference. In combination with a template, this is a
// universal reference; stuff can now be every type. Including references,
// const references, pointers, lambdas, etc.
template <typename Stuff>
static void inject(Stuff&& stuff)
{
	try {
		// We assume that () can be applied to stuff. If that is not
		// possible (e.g., when stuff is an int), the compiler might
		// emit some complex error. There is template magic (or C++20
		// concepts) to capture that, but we are still not going to
		// discuss it here.
		stuff();
	} catch(...) {
		// Caught some illness.
	}
}

// Template parameters can also be non-type things, like just values.  In this
// case, Stuff can be auto-deducted, but count cannot. The user has to
// explicitly specify count, as we will see in a moment.
template <int count, typename Stuff>
static void protect(Stuff& stuff)
{
	for(int i = count; i; --i)
		inject(stuff);
}

// Some thing to apply () to.
static void drug()
{
	throw "Party";
}





int main()
{
	// Instantiate some Vaccins. Check out how either Vector (aliases) or
	// mRNA types are passed to Vaccin.
	Vaccin<SputnikV> sputnikv;
	Vaccin<mRNA> pfizer;
	Vaccin<mRNA> moderna;
	Vaccin<Oxford> astrazeneca;
	Vaccin<JohnsonJohnson> janssen;
	// This defaults to Vaccin<void>.
	Vaccin<> placebo;

	// inject() is called without specifying the template parameter; it is
	// auto-deducted from the arguments that are passed to it.
	inject(pfizer);                     // Calls first overload of inject().
	inject(moderna);                    // Idem.
	inject(placebo);                    // Idem.
	inject(drug);                       // Calls second overload of inject().
	inject([](){ /* does nothing */});  // Idem.
	(void)sputnikv; // unused

	// Here, the first template parameter is specified explicitly, while
	// the second one is auto-deducted from the argument that is passed.
	protect<2>(astrazeneca);
	protect<1>(janssen);

	// Do some type checks...
	static_assert(is_vector<Vaccin<Oxford>>::value, "");
	static_assert(is_vector<decltype(astrazeneca)>::value, "");

	// As you (hopefully) noticed, although the implementation of the
	// Vaccin class and inject function might be somewhat complex, using it
	// is very easy. If you get the API right, you really can get it
	// intuitive**, while still allowing the compiler to do full
	// optimization.
}

/*
 * Further reading:
 *
 * - template member functions
 * - type traits
 * - SFINAE
 * - template using
 * - variadic template arguments
 * - ...
 *
 * ...but first make sure you understand this file before continuing on these
 * topics, so I won't give you any links (yet).
 */

