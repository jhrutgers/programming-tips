/*
 * std::optional
 *
 * When a function may or may not return a value, C++17 provides std::optional,
 * which expresses this either-the-function-was-successful-or-there-is-no-data-
 * to-be-returned behavior.
 *
 * Let's see how it works. Read from top to bottom.
 */

// This header is required for std::optional. The others are just for this
// example.
#include <optional>


// First a preamble...

#include <any>
#include <chrono>
#include <exception>
#include <functional>
#include <iostream>
#include <memory>
#include <random>
#include <string>
#include <tuple>
#include <utility>
#include <variant>

// For C programmers: this actually means: srand(time(NULL))
static std::default_random_engine generator(
	static_cast<unsigned>(std::chrono::system_clock::now().time_since_epoch().count()));

// For C programmers: toss() wraps a call like: rand() % 2
static std::uniform_int_distribution<int> distribution(0,1);
static auto toss = std::bind(distribution, generator);

using namespace std::literals;




// If you have a function that may or may not return data, you can do that in
// multiple ways. For example:

// When there is output, it is written to the argument, and true is returned.
// However, documentation is required to indicate when and how the data is
// returned.
static bool troll0(std::string& x)
{
	if(!toss())
		return false;

	x = "COVID-19 is just like the common flu (and I heard some "
		"self-claimed expert saying this, so I believe it immediately. I "
		"always believe random experts on social media, except those who "
		"have dedicated their life to science, because they don't always "
		"say what I like to hear)";

	return true;
}

// This may return a null pointer, or the data when it is available.  Wrapped
// in a unique_ptr makes sure that the user won't forget deleting it
// afterwards.  Documentation is required to indicate that one can get a
// nullptr, and the uniqueness is a bit distracting; why isn't it a shared_ptr?
// Not really nice solution.
static std::unique_ptr<std::string> troll1()
{
	return toss() ? std::make_unique<std::string>(
		"COVID-19 is created in a Chinese lab (and those guys are so "
		"smart in labs (but not in labs where vaccines are created) -- "
		"I'm sure, as it's got electrolytes)") :
		nullptr;
}

// This returns a pair where you should ignore the .first when .second is
// false.  Not too bad, but std::pair is very generic. You may want to typedef
// it, but it still needs documentation.
static std::pair<std::string, bool> troll2()
{
	return toss() ?
		std::make_pair("5G causes COVID-19 (and I can know, because I "
			"have seen an antenna once, so I'm an expert)"s, true) :
		std::make_pair(""s, false);
}

// This is the same as the std::pair solution, although it is a bit harder to
// use and a tuple looks even more generic than a pair.
static std::tuple<std::string, bool> troll3()
{
	return toss() ?
		std::make_tuple("COVID-19 is injected together with the flu "
			"vaccine (and I know that, because I cannot read and "
			"certainly do not understand the ingredient list, so it "
			"must be true)"s,
			true) :
		std::make_tuple(""s, false);
}

// A variant kind of allows you to have 'no value'. Note that accessing the
// value does some kind of type inspection, and has therefore some overhead,
// compared to std::pair, for example. Again, you probably want to typedef it
// to make the intention clear.
static std::variant<std::monostate, std::string> troll4()
{
	std::variant<std::monostate, std::string> res;
	return toss() ? res = "The COVID-19 vaccine will modify your DNA (and "
		"although I don't know what DNA or mRNA stands for, I know it's "
		"true, as I do understand all principles of vaccines, "
		"obviously)"s : res;
}

// Now, you don't have a clue what is returned, although you need to know that
// an std::string can be contained.  And it has some run-time overhead in
// accessing the data, like the std::variant. Not really nice, as it hides too
// much type information.
static std::any troll5()
{
	return toss() ? std::any{"The true cause of death of COVID-19 is "
		"thrombosis, and aspirin cures it (and all the medical staff on "
		"the IC missed this note on Facebook, which is stupid, as "
		"Facebook is known to be a reliable source of information -- I "
		"am certain, as I also read some story about trolls and elves "
		"online, which is also true)"s} : std::any{};
}

// Usually, you only use exceptions in case of abnormal program flow.
// Moreover, people may try to avoid exceptions in embedded or safety systems.
// So, this is not really a good solution either.
static std::string troll6()
{
	if(toss())
		return "The COVID-19 vaccine contains nanobots that let the "
			"government control you remotely (and the fact that you "
			"can't see them in the transparent vaccines, proves that "
			"they are really nano -- QED)"s;

	throw "Truth bomb";

	// Well, throwing an std::exception is better...
//	throw std::exception{};

	// Throwing some subclass of std::exception is even better, as it gives
	// a better indication of your intention.
//	throw no_data{};
}

// Using std::optional clearly indicates your intention. It will have no value,
// or an std::string in this case. std::optional's API is very straight-
// forward; just guess how you could use it, and it is probably fine.
static std::optional<std::string> elf0()
{
	std::optional<std::string> res;

	if(toss())
		res = "Garlic protects against COVID-19 (well, it doesn't do "
			"anything against a virus, but it will keep other "
			"people at a distance -- and that helps, especially if "
			"these people say really stupid things)"s;

	return res;
}

// The same idea as above, but now written a bit more compact.
static std::optional<std::string> elf1()
{
	if(toss())
		return "If you don't trust mRNA/vaccine technology, medicine, "
		       "statistics, telecommunication, and journalism, you "
		       "shouldn't trust you phone either.  Wake up."s;

	return std::nullopt;
}

int main()
{
	if(std::string x; troll0(x))
		std::cout << "arg:         " << x.c_str() << std::endl;

	// Pointer data
	if(auto x = troll1())
		std::cout << "unique_ptr:  " << x->c_str() << std::endl;

	if(auto x = troll2(); x.second)
		std::cout << "pair:        " << x.first.c_str() << std::endl;

	if(auto x = troll3(); std::get<bool>(x))
		std::cout << "tuple:       " << std::get<std::string>(x).c_str() << std::endl;

	if(auto x = troll4(); auto* p = std::get_if<std::string>(&x))
		std::cout << "variant:     " << p->c_str() << std::endl;

	if(auto x = troll5(); auto* p = std::any_cast<std::string>(&x))
		std::cout << "any:         " << p->c_str() << std::endl;

	try {
		auto x = troll6();
		std::cout << "exceptions:  " << x.c_str() << std::endl;
	} catch(...) {
	}

	// Things are getting better.
	if(auto x = elf0(); x.has_value())
		std::cout << "optional 1:  " << x.value().c_str() << std::endl;

	// Note the clean syntax.  The implicit cast of an std::optional to
	// bool is the same as has_value(). The pointer dereference and member
	// access operator access the value (or throws std::bad_optional_access
	// if it does not hold a value).
	if(auto x = elf1())
		std::cout << "optional 2:  " << x->c_str() << std::endl;

	// Basically, std::optional in this context helps you expressing the
	// intention of the program. In general, this is a good thing, as it
	// makes understanding the code easier, even without reading the
	// manual.
}

/*
 * Further reading:
 *
 * https://en.cppreference.com/w/cpp/utility/optional
 */
