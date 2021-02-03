/*
 * Templates
 *
 */

#include <string>
#include <vector>
#include <utility>

// Just different types.
struct ChAdOx1 {};
struct Ad26 {};
struct Ad5 {};

struct mRNA {
	void deliver() { /* ... */ }
	std::string sequence = u8"GAGAAΨAAAC...";
};

// Class with template.
template <typename Virus>
struct Vector {
	void deliver() { /* ... */ }
	std::vector<std::string> payload { "ATGTTC", "GTGTTCCTGG", "..." };
};

using Oxford = Vector<ChAdOx1>;
using JohnsonJohnson = Vector<Ad26>;
typedef Vector<std::pair<Ad26,Ad5>> SputnikV;

// Class with template with default value.
template <typename T = void>
class Vaccin {
public:
	using type = T;

	void work() { type().deliver(); }
};

// Specialization
template<>
class Vaccin<void> {
public:
	using type = void;
	void work() {}
};

// Overload for Vaccins (auto-deduct)
template <typename T>
static void inject(Vaccin<T>& vaccin) {
	vaccin.work();
}

// Overload for the rest, with universal reference.
template <typename Stuff>
static void inject(Stuff&& stuff) {
	try {
		stuff();
	} catch(...) {
		// Caught some illness.
	}
}

// Template value (not auto-deducted)
template <int count, typename Stuff>
static void protect(Stuff& stuff) {
	for(int i = count; i; --i)
		inject(stuff);
}

// Some thing to apply () to.
static void drug() {
	throw "Party";
}


int main()
{
	Vaccin<SputnikV> sputnikv;
	Vaccin<mRNA> pfizer;
	Vaccin<mRNA> moderna;
	Vaccin<Oxford> astrazeneca;
	Vaccin<JohnsonJohnson> janssen;
	Vaccin<> placebo;

	inject(pfizer);
	inject(moderna);
	inject(placebo);
	inject(drug);
	inject([](){ /* does nothing */});
	(void)sputnikv; // unused

	protect<2>(astrazeneca);
	protect<1>(janssen);

	// Much more to learn...
	// - template member functions
	// - type traits
	// - SFINAE
	// - template using
}

