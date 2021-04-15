/*
 * Enums
 *
 * Before you had enums, which are kind of ints with names. Since C++11, you
 * have strongly typed enums, namely enum classes. Let's dive into the
 * differences.
 *
 * Work you way down through this file. You don't really need to execute the
 * program.
 */

#include <cassert>
#include <iostream>
#include <type_traits>

// The old unscoped enum. The name may be omitted to make the enum anonymous,
// in case only the enumerators are needed.
enum Mode {
	Reset,
	Idle,
	PreFlight,
	Check,
	Flight,
};

// The new scoped enum. 'enum class' is the same as 'enum struct'.
enum class Test {
	Deploy,
	LowSpeed,
	HighSpeed,
	Flight,
};

// By default, an enum (class) is an int or bigger, depending on the enumerator
// values. However, you can explicitly specify the underlying type.
//
// For normal (non-fixed typed) enums and enum classes, the range of integers
// that can be used is 0 to pow(2,ceil(log2(largest value)))-1, in this case
// [0,4095].  Casting values outside of this range, is undefined behavior.
//
// For fixed-typed enums and enum classes, the full range of the underlying
// type can be used.
enum class Speed : uint16_t {
	Off = 0,
	LowSpeed = 50,
	Flight = 2400,
	HighSpeed = 2537,
	MaxSpeed = HighSpeed,
};

// A user-defined literal to convert an integral type to a Speed.
static Speed operator ""_rpm(unsigned long long i)
{
	// Just checking... I don't like undefined behavior, I need my CPU for
	// a while...
	assert(i <= static_cast<int>(Speed::MaxSpeed));

	// Does not work: must be explicitly initialized to Speed.
	//
	// return i;

	// Does not work: narrowing type is not allowed (unsigned long long
	// should be narrowed to uint16_t).
	//
	// return Speed{i};

	// This does work:
	return Speed{static_cast<std::underlying_type_t<Speed>>(i)};
}

// You can do operator overloading with enums. Enums can be converted
// implicitly to an int, but enum classes can't.  So, especially for enum
// classes, operator overloading may be interesting.
Speed& operator^(Speed& speed, Test& test)
{
	switch(test) {
	case Test::LowSpeed:
		return speed = Speed::LowSpeed;
	case Test::HighSpeed:
		return speed = Speed::HighSpeed;
	case Test::Flight:
		return speed = Speed::Flight;
	default:
		return speed = Speed::Off;
	}
}

int main()
{
	// Mode is the old (pre-C++11) enum. Note that its enumerators are
	// added to the surrounding scope.
	Mode mode = Reset;

	// For enum classes, you always have to specify the scope.
	Speed speed = Speed::Off;


	// Let's do some tests.

	// In C++17, you can do brace-initializer-like initialization.  (OK,
	// strictly, this is an initializer-list with one element, but it looks
	// the same.)
	Test test{Test::Deploy};

	// Since C++11, you can specify the 'scope' of the unscoped (old) enum.
	// This is identical to omitting the enum name, though, since the
	// enumerators are part of the current scope also.
	mode = Mode::Idle;


	// So far, so good...
	test = Test::LowSpeed;
	mode = PreFlight;
	speed ^ test; // ...some arbitrary operator chosen for this example.
	// Great, it works.


	// The old enum allows implicit conversion to integral types.
	int i = mode;
	// Converting from an integral type to the enum type must be explicit.
	mode = (Mode)(i + 1);
	// So, this will call the operator<< with an int argument for mode.
	std::cout << "mode = " << mode << std::endl;



	// This is a bit iffy, though. Enums are often used for explicit and
	// named values.  In case of the Mode enum, this is unwanted and asking
	// for troubles.
	//
	// However, for the Speed enum, it may be sensible to do. In this case,
	// the enum class is a kind of special integral value; it is limited in
	// range (uint16_t), has proper operator overloading, and has a few
	// predefined values. So, this makes sense:
	speed = 2399_rpm;

	// The new enum does not allow this implicit conversion to an integral
	// type. Therefore, it is considered strongly typed. So, this will give
	// a compile error:
	//
	// std::cout << "speed = " << speed << std::endl;
	//
	// This is fine:
	std::cout << "speed = " << static_cast<int>(speed) << std::endl;

	// This looks ambiguous; is it Mode::Flight, Test::Flight, or
	// Speed::Flight?  As Mode is the only unscoped enum, it is clear for
	// the compiler, but it might be confusing for the reader. To prevent
	// crashes, I would suggest to prefer specifying the scope, even for
	// unscoped enums.
	mode = Flight;

	// Although the enum class is 'strongly typed', this is still allowed
	// and equivalent:
	test = static_cast<Test>(3);
	test = (Test)3;                // the same as static_cast
	test = Test{3};                // C++17 brace-init
	// However, it is still asking for troubles, as Test is intended to
	// have just a few discrete values.

	// In this example, Mode should be fixed; enum class would be better.
	// A firmware update is required...
	mode = (Mode)0;


	// Summarized, using an enum class is nice because of the scoping, but
	// normal enums are still useful, for example for macro-like
	// definitions:

	enum Ingenuity {
		Height_cm = 49,
		Diameter_cm = 120,
		Mass_g = 1800,
		SoftwareVersion = 2,
	};

	return SoftwareVersion;
}

/*
 * Further reading:
 *
 * https://en.cppreference.com/w/cpp/language/enum
 */

