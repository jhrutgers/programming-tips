/*
 * Substitution Failure Is Not An Error
 *
 * When using templates for functions, you may encounter the situation where
 * depending on the type, a function is allowed to be used or not. So, you may
 * want to do some analysis on the template type, and allow the compiler to use
 * or discard a specific function.  This technique is called SFINAE. It existed
 * in the pre-C++11 days, but since C++11 the standard library adds many
 * utilities to make it easier to use SFINAE.
 */

#include <iostream>
#include <limits>

struct Person {
	typedef int can_walk;
	enum { vaccinated = 0 };
};

struct USCitizen : public Person {
	typedef int living_in_us;
};

template <typename P = Person>
struct FearfulPerson : public P {
	typedef int afraid_of_water;
	typedef int living_in_mountain;
	enum { vaccinated = 1 };
};

template <typename P = Person>
struct ThrillSeeker : public P {
	typedef int flying;
	typedef int motor_cycling;
};

double motorcycle_accident(Person const& person) {
	return std::numeric_limits<double>::infinity();
}

// More specific overload, when enabled.
template <typename P, typename P::motor_cycling = 0>
int motorcycle_accident(P&& person) {
	return 70072;
}

// In return type
template <typename P>
typename P::can_walk traffic_accident_as_pedestrian(P&& person) {
	return 42600;
}

// As argument
template <typename P>
int firearm(P&& person, typename P::living_in_us = 0) {
	return 23439;
}

template <typename A, typename B> struct is_same      { using type = void; };
template <typename A>             struct is_same<A,A> { using type = int;  };

template <typename P, typename is_same<P,Person>::type = 0>
int fall_from_ladder(P&& person) {
	return 674572;
}

template <typename P, typename std::enable_if<!P::vaccinated, int>::type = 0>
int die_from_covid19(P&& person) {
	return 1016;
}

template <typename P, typename std::enable_if<P::vaccinated, int>::type = 0>
double die_from_covid19(P&& person) {
	return std::numeric_limits<double>::infinity();
}

template <typename P, typename std::enable_if<std::is_trivial<P>::value, int>::type = 0>
int die_from_astrazeneca(P&& person) {
	return 656250;
}

int main()
{
	std::cout << "Chance of dying within one year" << std::endl;
	std::cout << "Motorcycle riders by motor cycle accident:      1 in " << motorcycle_accident(ThrillSeeker<>()) << std::endl;
	std::cout << "Non-motorcycle riders by motor cycle accident:  1 in " << motorcycle_accident(FearfulPerson<>()) << std::endl;
	std::cout << "Traffic accident as pedestrian:                 1 in " << traffic_accident_as_pedestrian(USCitizen()) << std::endl;

	// Compile error
//	std::cout << "Traffic accident as a string:                   1 in " << traffic_accident_as_pedestrian(std::string()) << std::endl;

	std::cout << "Assault by firearm:                             1 in " << firearm(ThrillSeeker<USCitizen>()) << std::endl;
	std::cout << "Fall from ladder:                               1 in " << fall_from_ladder(Person()) << std::endl;

	// Compile error
//	std::cout << "Fall from ladder if you can float:              1 in " << fall_from_ladder(42.f) << std::endl;

	std::cout << "COVID-19 in the Netherlands without vaccine:    1 in " << die_from_covid19(ThrillSeeker<>()) << std::endl;
	std::cout << "COVID-19 in the Netherlands with vaccine:       1 in " << die_from_covid19(FearfulPerson<>()) << std::endl;
	std::cout << "COVID-19 vaccine from AstraZeneca:              1 in " << die_from_astrazeneca(Person()) << std::endl;
}

/*
 * Further reading:
 *
 * https://en.cppreference.com/w/cpp/language/sfinae
 * https://en.cppreference.com/w/cpp/header/type_traits
 *
 * https://www.iii.org/fact-statistic/facts-statistics-mortality-risk
 */

