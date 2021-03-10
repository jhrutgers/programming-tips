/*
 * literals
 *
 * Literals like 42, or "foo" are used by everyone. And probably you know there
 * is something with a suffix like u or ll.  However, there is a lot more.
 *
 * Scroll down to main() and follow the execution flow. You probably want to
 * run the program to see what happens.
 */

#include <iostream>
#include <iomanip>
#include <regex>
#include <locale>
#include <codecvt>
#include <chrono>

#ifdef WIN32
#  include <windows.h>
#endif

template<class I, class E, class S> struct codecvt : std::codecvt<I, E, S> { ~codecvt() { } };

template <typename T>
void print_density(char const* party, T bpp) {
	std::cout
		<< "Election manifesto density of " << party
		<< " is " << std::fixed << std::setprecision(1) << bpp
		<< " bytes per page." << std::endl;
}

// A user-defined literal is implemented as the "" operator. The suffix has to
// start with an underscore. The argument to the operator can be (among a few
// variations):
//
// - unsigned long long int: any integer literal
// - long double:            any floating point literal
// - char const*, size_t:    string literal
//
// It can return anything. Although it looks a bit strange, a literal 13_fvd
// will be replaced by a call to operator""_fvd(13) and return a char const* in
// this example.
char const* operator ""_fvd(unsigned long long fact) {
	return "only 151 young";
}

std::string operator""_fvd(char const* fact, size_t size) {
	return std::regex_replace(fact, std::regex("is a .*"), "is a hoax");
}

std::string operator""_pvv(char const* fact, size_t size) {
	return std::regex_replace(fact, std::regex("Climate change"), "Islamification");
}

std::string operator""_sp(char const* fact, size_t size) {
	return std::regex_replace(fact, std::regex("limited to"), "increased to way above");
}

int operator ""_sgp(long double fact) {
	return 0;
}

std::string operator""_denk(char const* fact, size_t size) {
	return std::regex_replace(fact, std::regex("compensated for (.*)"), "resilient to $1; we increase the gas production to compensate for other losses");
}

std::string operator""_30(char const* fact, size_t size) {
	return std::regex_replace(fact, std::regex("back .*"), "more destruction. Who believes science anyway?");
}

struct Volt {
	long double x;
	Volt operator+(Volt const& rhs) const { return Volt{x + rhs.x}; };
	Volt& operator+=(Volt const& rhs) { x += rhs.x; return *this; }
};

std::ostream& operator<<(std::ostream& os, Volt const& val) {
	return os << val.x << " V";
}

Volt operator""_V(unsigned long long x) {
	return Volt{(long double)x};
}

Volt operator""_V(long double x) {
	return Volt{x};
}

int main()
{
	// These you already know.
	int vvd = 32; // decimal
	int pvv = 030; // octal
	int cda = 0x11; // hex

	// This is new: binary literal, but it looks familiar.
	int ja21 = 0b1000 - 0b101;

	// This is floating.
	double krol = 4.01;
	double pvdt = 4007e2-4.155e2 + krol;
#if __cplusplus >= 201703L
	// Fractional part is optional, but the exponent is not. The exponents
	// starts with a p, instead of the usual e, because an e would be part
	// of the hex value itself. The value of exponent is always in decimal.
	pvdt += 0x77p2;
	double go = pvdt - 0x61BA082.8F5C3p-2;
	(void)go; // Where did it go?
#endif
	(void)pvdt; // Gone also.
	(void)(vvd + pvv + cda + ja21); // Is never going to work.


	// As you know, you can also have string literals. But choose them
	// wisely, as picking one arbitrarily may have some nasty side-effects.

#ifdef WIN32
	// First of all, Windows needs some more attention to print non-ASCII
	// to the console... Argh...
	SetConsoleOutputCP(CP_UTF8);
#endif

#if _MSC_VER < 1900 || _MSC_VER >= 1920
	// Don't use UTF-16 or UTF-32, as for every ASCII character (probably
	// common case), 0 bytes are inserted. It is inefficient. And annoying
	// to print.  And has a bug in MSVC 2015 and 2017.
	char16_t const Magda[] = u"Łuczycki";
	std::cout << std::wstring_convert<codecvt<char16_t,char,std::mbstate_t>,char16_t>().to_bytes(Magda) << " = " << sizeof(Magda) << " bytes" << std::endl;

	char32_t const Gurcu[] = U"Polat-Iş\u0131ktaş";
	std::cout << std::wstring_convert<codecvt<char32_t,char,std::mbstate_t>,char32_t>().to_bytes(Gurcu) << " = " << sizeof(Gurcu) << " bytes" << std::endl;
#endif

	// Don't use wchar_t (wide strings), as it is locale and OS dependent
	// what you get...
	wchar_t const Dilan[] = L"Yeşilgöz-Zegerius";
	std::cout << std::wstring_convert<std::codecvt_utf8<wchar_t>,wchar_t>().to_bytes(Dilan) << " = " << sizeof(Dilan) << " bytes" << std::endl;
	// ...and codecvt_utf8 is deprecated in C++17.

	// Always standardize to UTF-8. It's easy, consistent, and efficient.
	// This is a good choice.
	//
	// However, this is awkward. char8_t is new and broken in C++20. This
	// is what you want to do, but std::cout cannot accept char8_t -- it is
	// explicitly removed from the standard, but there is no alternative
	// yet.
//	char8_t const Meyrem[] = u8"Çimen";
#ifndef __cpp_char8_t
	// C++17 and earlier did not know char8_t, so u8"" just returns char.
	// This can be handled properly.
	char const Meyrem[] = u8"Çimen";
#else
	// Till C++23(?), don't use the u8 prefix. Just do this:
	char const Meyrem[] = "Çimen";
#endif
	std::cout << Meyrem << " = " << sizeof(Meyrem) << " bytes" << std::endl;

	// Ahhh, strings, why are they so difficult?!? Ok, remember this: Only
	// use UTF-8 (and safe your source files accordingly), normal string
	// literals, and just char, and everything works fine.

	// By the way, the standard defines that you can use Unicode characters
	// in names too.  However, g++ < 10, cppcheck, and probably other
	// tools, don't seem to support it. Anyway, the following is correct
	// C++, but I still do not recommend it...
//	char const Songül[] = "Mutluer";



	// Values can have suffixes, namely l or L (long or long double), ll or
	// LL (long long), u or U (unsigned, combined with L or LL), f or F
	// (float). That was already there for a long time.  What was added in
	// C++14, is that you can add an ' anywhere in between digits for
	// readability. They are ignored by the compiler.
	long double d66_bpp  =   1'985'326L  / 208.0L;
	double      pvda_bpp =  15'890'202LL / 105.;
	float       sp_bpp   =     178'223   /  32.f;
	print_density("D66", d66_bpp);
	print_density("PvdA", pvda_bpp);
	print_density("SP", sp_bpp);

	// In the example above, the unit is a suffix to the name, just as an
	// indication to the reader, but for the compiler it still is just a
	// number. However, we can enforce actual types on literals, as C++
	// allows you to define any suffix. The standard library as a few for
	// you, notably:
	using namespace std::chrono_literals;
	auto remaining_campaign = 24h * 3; // There is no 'd' suffix for days, though.
	auto voting_window = 13.5h + 810min + 48600s;
	std::cout
		<< "Still campaigning for "
		<< std::chrono::duration_cast<std::chrono::duration<long,std::ratio<86400>>>(remaining_campaign).count() << " days"
		<< " to get people voting within roughly "
		<< std::chrono::duration_cast<std::chrono::hours>(voting_window).count() << " hours" << std::endl;



	// It becomes better; you can define your own suffix!  This opens a
	// whole new world of possibilities. Let's see how this freedom can be
	// used.

	std::cout << "Wilders: \"" << "Climate change is our biggest challenge"_pvv << "\"" << std::endl;
	std::cout << "Baudet: \"" << "COVID-19 is a deadly virus"_fvd << "\"" << std::endl;
	std::cout << "Baudet: \"In 2020, " << 11'432_fvd << " people died because of corona\"" << std::endl;
	std::cout << "SP: \"" << "The national dept should be limited to sane levels"_sp << "\"" << std::endl;
	std::cout << "SGP: \"Men and women are equal. Therefore we want " << 50.0_sgp << "% of the candidates to be female\"" << std::endl;
	std::cout << "DENK: \"" << "People in Groningen should be compensated for the effects of gas extraction"_denk << "\"" << std::endl;
	std::cout << "Blanco: \"Vaccines will bring us back to normal\""_30 << std::endl;



	// Ok, such a usage of user-defined literals is just wicked.  It is
	// better to stick to scientific usages, especially to use SI units.
	// Basically, this is how you could implement it:
	auto x = 17_V;

	// An expression like x + 2s or 1_V + 5 is now prohibited by the
	// compiler.  If such a physics library implement the units correctly,
	// it will nicely protect you of doing computations that make no sense.
	std::cout << x + 2.8e1_V << std::endl;

	// It is probably a good idea to pick one of the available unit
	// libraries (usually header-only and only compile-time overhead), and
	// use it everywhere when you do anything with physics.
}

/*
 * Further reading:
 *
 * https://en.cppreference.com/w/cpp/language/integer_literal
 * https://en.cppreference.com/w/cpp/language/floating_literal
 * https://en.cppreference.com/w/cpp/language/user_literal
 *
 * A library that defines suffixes for all SI units, including checks on math
 * on these values:
 * https://github.com/nholthaus/units
 */
