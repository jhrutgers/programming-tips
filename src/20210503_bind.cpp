/*
 * Structured bindings
 */

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
	template <size_t i>
	std::enable_if_t<i < 6u, Module const&> get() const noexcept {
		switch(i) {
		case 0: return zarya;
		case 1: return unity;
		case 2: return destiny;
		case 3: return harmony;
		case 4: return tranquility;
		case 5: return beam;
		default: std::terminate();
		}
	}
};

template <>
struct std::tuple_size<ISS> { enum { value = 6 }; };

template <size_t i>
struct std::tuple_element<i, ISS> { using type = Module; };

int main()
{
	// Heavenly Palace
	int tiangong_$[2]{2011, 2016};
	// must be auto
	auto [ tg1, tg2 ] = tiangong_$;

	// Tuple
	Tiangong tiangong{{2021, "Long March 5B"}, {2022}, {2022}};
	auto& [ t, w, m ] = tiangong;
	m.launch = 2023;

	// Struct member
	// note, cannot have members in both Module base and Tiange subclass.
	auto const& [ launched, vehicle ] = t;
	std::cout << "Launched Tiange in " << launched << " by " << vehicle << std::endl;

	// Tuple-like
	ISS iss;
	auto const& [ m1, m2, m3, m4, m5, m6 ] = iss;
	std::cout << "First ISS module launched in " << m1.launch << " by " << m1.by << std::endl;
}

