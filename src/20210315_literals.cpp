/*
 * literals
 */

#include <iostream>
#include <regex>
#include <locale>
#include <codecvt>

#if 0
long double operator""_π(long double x)
{
	return x * 3.1415926535897932384626433832795029L;
}

std::string operator""_c(char const* str, size_t len)
{
	return std::regex_replace(str, R"/(r)/", "l");
}
#endif

template<class I, class E, class S> struct codecvt : std::codecvt<I, E, S> { ~codecvt() { } };

int main()
{
	// These you already know.
	int vvd = 32; // decimal
	int pvv = 030; // octal
	int cda = 0x11; // hex

	// This is new: binary literal, but it looks familiar.
	int ja21 = 0b1000 - 0b101;

	double krol = 4.01; // floating
	double pvdt = 4007e2-4.155e2 + krol;
#if __cplusplus >= 201703L
	// Fractional part is optional, but the exponent is not. The exponents
	// starts with a p, instead of the usual e, because an e would be part
	// of the hex value itself. The value of exponent is always in decimal.
	pvdt += 0x77p2;
	double go = pvdt - 0x61BA082.8F5C3p-2;
	(void)go;
#endif

#if 0
	// suffix
	1.2 // double
	1.2f // float
	1.2l // long double
	1u // unsigned
	1l // long
	1ll // long long
#endif

	// Don't use UTF-16 or UTF-32, as for every ASCII character (probably
	// common case), 0 bytes are inserted. It is inefficient. And annoying to print.
	char16_t const Magda[] = u"Łuczycki";
	std::cout << std::wstring_convert<codecvt<char16_t,char,std::mbstate_t>,char16_t>().to_bytes(Magda) << " = " << sizeof(Magda) << std::endl;

	char32_t const Murat[] = U"Memi\u015f"; // Memiş
	std::cout << std::wstring_convert<codecvt<char32_t,char,std::mbstate_t>,char32_t>().to_bytes(Murat) << " = " << sizeof(Murat) << std::endl;

	// Don't use wchar_t (wide strings), as it is locale and OS dependent what you get...
	wchar_t const Dilan[] = L"Yeşilgöz-Zegerius";
	std::cout << std::wstring_convert<std::codecvt_utf8<wchar_t>,wchar_t>().to_bytes(Dilan) << " = " << sizeof(Dilan) << std::endl;
	// ...and codecvt_utf8 is deprecated in C++17.

	// Always standardize to UTF-8.
#if defined(__GNUC__) && __GNUC__ < 10
	// g++ < 10 does not support Unicode characters in variable names,
	// which is defined by the standard...
	char const Gurcu[] = u8"Polat-Işıktaş";
	std::cout << Gurcu << " = " << sizeof(Gurcu) << std::endl;
#else
	// ...but good luck with typing variables names without Unicode
	// keyboard -- you should probably stick to only standard ASCII
	// characters for names.
	char const Gürcü[] = u8"Polat-Işıktaş";
	std::cout << Gürcü << " = " << sizeof(Gürcü) << std::endl;
#endif


	(void)vvd;
	(void)pvv;
	(void)cda;
	(void)ja21;
	(void)pvdt;

#if 0
	// separator (C++14)
	123'456

	'c' // char
	"str" // string

	// std suffixes
	3.4i // <complex>
	4s // <chrono>

	// si (library)

	// user defined
#endif
}

/*
 * https://github.com/nholthaus/units
 */

