/*
 * std::any
 *
 * C++17 adds the std::any container, that can hold any object of any type, and
 * can switch contained types at any time. std::any implements type erasure; it
 * embeds any type, but the interface is the same.  No templates, nothing
 * virtual at the std::any interface, but still type-safe on the contents.
 * Let's have a look how you can use it.
 *
 * Scroll down to main() and start reading from there. Execution of the program
 * is nice, but not essential.
 */

// std::any is defined by this header. The other headers below are just for
// this example.
#include <any>

#include <functional>
#include <iostream>
#include <vector>

static void show(std::any const& song)
{
	// If any_cast is passed a pointer to std::arg instead of a reference,
	// it will return a pointer. This pointer is nullptr in case of a bad
	// cast, without throwing an exception.  This is a better way to check
	// if the contained type is what you expect.
	if(auto const* si = std::any_cast<int>(&song)) {
		std::cout << *si << " (int)" << std::endl;
	} else if(auto const* sc = std::any_cast<char const*>(&song)) {
		std::cout << *sc << " (char const*)" << std::endl;
	} else if(auto const* ss = std::any_cast<std::string>(&song)) {
		std::cout << *ss << " (std::string)" << std::endl;
	} else {
		// You can also get the type() of the std::any, which returns
		// an std::type_info. This is the same thing as returned by
		// typeof(). Then, you can compare types based on this object,
		// or request the (mangled) name.
		std::cout << "(unknown type " << song.type().name() << ")" << std::endl;
	}
}

int main()
{
	// Lets define an std::any instance, that holds an int.
	std::any ahoy = 65;
	// To extract something from std::any, use the std::any_cast, where you
	// specify what type you expect to be in there.
	std::cout << std::any_cast<int>(ahoy) << std::endl;

	// We can just reassign another type to the same any instance.
	ahoy = "esc2020";
	// Again, use std::any_cast to get the contents, but in this case, the
	// type is different.
	std::cout << std::any_cast<char const*>(ahoy) << std::endl;

	// You can also destruct the contained object explicitly by calling
	// reset().  ahoy.has_value() will return false afterwards.
	ahoy.reset();

	// ...and reassign again.
	ahoy = std::string("Shine a light");
	std::cout << std::any_cast<std::string>(ahoy) << std::endl;


	// So far, so good. There are, however, some ugly details.  To use
	// std::any_cast, you need to know the type. You can't just do
	// something like std::any_cast<char const*>(ahoy), and expect some
	// kind of static_cast to whatever you ask.

	// Consider this lambda, of which the exact type is unspecified.
	auto lordi = [](){ return "Hard Rock Hallelujah"; };
	std::any esc2006 = lordi;
	// What type is in the esc2006 container?
	try {
		// Although a lambda can be put in a std::function, this won't
		// work:
		std::cout << "Lordi: " << std::any_cast<std::function<char const*()>>(esc2006)() << std::endl;
	} catch(std::bad_any_cast const& e) {
		// In fact, it is considered a bad cast.
		std::cout << e.what();
	}

	// In this case, you want to be explicit what type is put in the
	// std::any.  For this, use std::make_any, which has a template
	// parameter that allows you to specify it.
	esc2006 = std::make_any<std::function<char const*()>>(lordi);

	// Now, the any does not hold the lambda directly, but it is wrapped in
	// an std::function (which in itself also does type erasure). This will
	// work properly:
	std::cout << "; " << std::any_cast<std::function<char const*()>>(esc2006)() << std::endl;

	// If you are suddenly confronted with an object, of which you don't
	// know the type, you could inspect it a bit better.
	//
	// As side note, the syntax of the if-statement below is valid for
	// C++17, but is really strange to 14. The for loop-like
	// initialization/condition now also works for if and switch.
	if(std::any wurst{Rise_like_a_Phoenix}; wurst.has_value()) {
		// Don't know what this is. Let's try to find out.
		show(wurst);
	}

	char Toy[] = "Toy";
	std::string Duncan{"Arcade"};

	// Using std::any, you can do things like having a vector with
	// different types.  So, declaring this is fine...
	std::vector<std::any> songs = { lordi, 1944, "Amar pelos dois", Toy, Duncan };

	// ...using it is a bit trickier, as you don't know the type of the
	// elements within the vector.
	for(auto const& song : songs)
		show(song);

	// So, I'm not completely convinced that this is really nice.  Usually,
	// I just have a look for some amusement. A lot of it is what you
	// recognize and expect, though the ugly parts make it dubious to some
	// extend.
	//
	// Anyway, about std::any, usually when you need type_info,
	// dynamic_cast, or other ways of run-time type inspection, it is often
	// an indication of a poor design. It is better to tell the compiler
	// what type you need, than ask in run-time what type you got. So, use
	// it with care.
}

/*
 * Further reading:
 *
 * https://en.cppreference.com/w/cpp/utility/any
 *
 * More about type erasure:
 * https://quuxplusone.github.io/blog/2019/03/18/what-is-type-erasure/
 */
