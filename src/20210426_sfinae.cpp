/*
 * Substitution Failure Is Not An Error
 *
 * When using templates for functions, you may encounter the situation where
 * depending on the type, a function is allowed to be used or not. It is better
 * to do this analysis at compile time, than to use assert()s at run time.  So,
 * you may want to do some analysis on the template type, and allow the
 * compiler to use or discard a specific function.  This technique is called
 * SFINAE. It existed in the pre-C++11 days, but since C++11 the standard
 * library adds many utilities to make it easier to use SFINAE.
 *
 * Read this file top to bottom. Executing it is not really required, although
 * it might give a nice overview of the numbers in the example.
 */

// SFINAE is defined in the language, no includes are required, although the
// type_traits is probably what you would like to include anyway.
#include <iomanip>
#include <iostream>
#include <limits>
#include <type_traits>

// Let's define a few types to work with.
struct Person {
	using can_walk = int;
	enum { vaccinated = 0 };
};

struct USCitizen : public Person {
	using living_in_us = int;
};

template <typename P = Person>
struct FearfulPerson : public P {
	using afraid_of_water = int;
	enum { vaccinated = 1 };
};

template <typename P = Person>
struct ThrillSeeker : public P {
	using flying = int;
	using motor_cycling = int;
	bool living_near_water;
};


// This is a normal function.
double motorcycle_accident(Person const& person)
{
	return std::numeric_limits<double>::infinity();
}

// This is an overload of the motorcycle_accident() function above.  In this
// case, we only want to be able to call this function when the given type P
// has the type motor_cycling. Although there are other ways of achieving this,
// let's try to do it with SFINAE. If this function is called like:
//
// motor_cycle_accident(ThrillSeeker<>{});
//
// ...then the template type P is deduced as ThrillSeeker<Person>.  Next, the
// second template parameter will be:
//
// ThrillSeeker<Person>::motor_cycling = 0
//
// This defines an non-type template parameter, in this case an int.  It does
// not receive a name, but it is initialized to 0.
//
// So far so good. When it is called like this:
//
// motor_cycle_accident(FearfulPerson<>{});
//
// ...then P is FearfulPerson<Person>, and the second template parameter will
// be:
//
// FearfulPerson<Person>::motor_cycling = 0
//
// However, FearfulPerson does not have the type motor_cycling. You would
// expect a compiler error indicating this, but now the SFINAE kicks in,
// because Substitution (of the template in the function type) Failure (the
// type does not exist) Is Not An Error; this function overload will just be
// discarded. In case of the FearfulPerson, the other overload is the only one
// that can be called.  In case of ThrillSeeker, this overload is more specific
// than the other one, so this overload will be chosen.
//
// This rule only applies to (member) functions with templates, when
// substituting the template type would make the function type ill-formed. A
// compile error in the function body is still just an error and not subject to
// SFINAE.
template <typename P, typename P::motor_cycling = 0>
int motorcycle_accident(P&& person)
{
	return 70072;
}

// In the function definition above, the SFINAE is implemented as another
// template argument.  The user should not specify it, but someone might be
// confused. If that bothers you, you can also try this method: put the SFINAE
// part in the return type of the function.
//
// In this case, we want to test P, which we expect to be a Person, whether it
// has the can_walk type. We know it is an int, so if it is there, the return
// type will be int and everything is OK.  If P does not have this field, for
// example if an std::string is supplied as parameter, std::string::can_walk
// will be a compile error and this function will be discarded as overload
// alternative.
//
// This works fine, but may make the API less readable. Moreover, this trick
// cannot be applied to constructors, since constructors don't have return
// types. Decide what works best for you.
template <typename P>
typename P::can_walk traffic_accident_as_pedestrian(P&& person)
{
	return 42600;
}

// You can also do the SFINAE thing as function parameter. In this example,
// when P is a (subclass of) USCitizen, the second function parameter can be
// resolved properly.  As it is default initialized to 0, you don't have to
// provide it when calling the function.  Again, when the type P does not have
// a type living_in_us, the function type will be ill-formed, and discarded
// silently for overload resolution.
//
// Whether you do the SFINAE trick as template parameter, function return type,
// or function parameter, it behaves the same. However, it may look better or
// worse, depending on the context.
template <typename P>
int firearm(P&& person, typename P::living_in_us = 0)
{
	return 23439;
}

// Just another example. Taking the address of a member of the class P only
// works if that member exists. There are more rules when SFINAE applies, such
// as arrays with negative size, pointer to a reference, etc. Generally,
// everything that will make the compiler usually bark on your code.
template <typename P>
int flood(P&& person, bool P::* = &P::living_near_water)
{
	return 7435624;
}

// The examples above test if a member of a type exists. However, there are
// many more properties you may want to test. For example, this type trait
// tests whether two types are identical. So, when called like:
//
// is_same<int,Person>::type
//
// ...the type will be void. And in this case, it will be int:
//
// is_same<float*,float*>::type
template <typename A, typename B> struct is_same      { using type = void; };
template <typename A>             struct is_same<A,A> { using type = int;  };

// Now, depending on P, the is_same<...>::type will be either void or int.  In
// case of void, the function is ill-formed; the template parameter void = 0 is
// invalid. So, this function can only be called when P is a Person.
template <typename P, typename is_same<P,Person>::type = 0>
int fall_from_ladder(P&& person)
{
	return 674572;
}

// In the standard library, there are many helper types like the is_same for
// this purpose.  Especially std::enable_if is regularly used. This struct has
// two template parameters: a bool, and a type. enable_if::type only exists
// when the first template parameter is true.  And if it exists, it will be the
// type of the second template parameter (by default void when not specified).
//
// So, when P has a member vaccinated that can be converted to a bool with the
// value false, this function can be called. If the vaccinated field does not
// exist at all, the function type is ill-formed and discarded.
template <typename P, typename std::enable_if<!P::vaccinated, int>::type = 0>
int die_from_covid19(P&& person)
{
	return 1016;
}

// When P has a member vaccinated that can be converted to a bool and is true,
// this function participates in the overload resolution. In this case, there
// are two overloads of this function, which are mutually exclusively enabled.
// So, either this or the other one is valid (or none, in case P is not a
// Person).
template <typename P, typename std::enable_if<P::vaccinated, int>::type = 0>
double die_from_covid19(P&& person)
{
	return std::numeric_limits<double>::infinity();
}

// The standard library has many type traits. Check it out at:
// https://en.cppreference.com/w/cpp/header/type_traits
template <typename P, typename std::enable_if<std::is_trivial<P>::value, int>::type = 0>
int die_from_astrazeneca(P&& person)
{
	return 656250;
}

// The syntax is a bit simplified by using variable templates (C++17) and
// helper types (C++14).
template <typename P, std::enable_if_t<std::is_same_v<typename P::living_in_us, int>, int> = 0>
int die_from_johnsonjohnson(P&& person)
{
	return 3400000;
}

// Now, go back to the 20210405_pack programming tip, navigation mark {india}
// for another example.

// More numbers for reference.
int hit_by_lightning() { return 14224671; }
int driving() { return 8303; }
int cataclismic_storm() { return 4304835; }

int main()
{
	std::cout << "Chance of dying within one year" << std::endl << std::endl;

	std::cout << "COVID-19 in the Netherlands without vaccine:    1 in " << die_from_covid19(ThrillSeeker<>{}) << std::endl;
	std::cout << "Any motor cycle traffic accident:               1 in " << driving() << std::endl;
	std::cout << "Assault by firearm:                             1 in " << firearm(ThrillSeeker<USCitizen>{}) << std::endl;
	std::cout << "Traffic accident as pedestrian:                 1 in " << traffic_accident_as_pedestrian(USCitizen{}) << std::endl;

	// Compile error
//	std::cout << "Traffic accident as a string:                   1 in " << traffic_accident_as_pedestrian(std::string{}) << std::endl;

	std::cout << "Motorcycle riders by motor cycle accident:      1 in " << motorcycle_accident(ThrillSeeker<>{}) << std::endl;

	std::cout << "COVID-19 vaccine from AstraZeneca:              1 in " << die_from_astrazeneca(Person{}) << std::endl;
	std::cout << "Fall from ladder:                               1 in " << fall_from_ladder(Person{}) << std::endl;

	// Compile error
//	std::cout << "Fall from ladder if you can float:              1 in " << fall_from_ladder(42.f) << std::endl;

	std::cout << "COVID-19 vaccine from Johnson&Johnson:          1 in " << die_from_johnsonjohnson(USCitizen{}) << std::endl;
	std::cout << "Twister-like force of nature:                   1 in " << cataclismic_storm() << std::endl;
	std::cout << "Flood:                                          1 in " << flood(ThrillSeeker<>()) << std::endl;
	std::cout << "Lightning:                                      1 in " << hit_by_lightning() << std::endl;

	std::cout << "Non-motorcycle riders by motor cycle accident:  1 in " << motorcycle_accident(FearfulPerson<>{}) << std::endl;
	std::cout << "COVID-19 in the Netherlands with vaccine:       1 in " << die_from_covid19(FearfulPerson<>{}) << std::endl;
}

/*
 * Further reading:
 *
 * https://en.cppreference.com/w/cpp/language/sfinae
 * https://en.cppreference.com/w/cpp/header/type_traits
 *
 * https://www.iii.org/fact-statistic/facts-statistics-mortality-risk
 */

