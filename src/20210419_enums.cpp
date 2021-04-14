/*
 * Enums
 *
 * Before you had enums, which are kind of ints with names. Since C++11, you
 * have strongly typed enums, namely enum classes. Let's dive into the
 * differences.
 */

#include <cassert>
#include <iostream>
#include <type_traits>

enum Mode {
	Reset,
	Idle,
	PreFlight,
	Check,
	Flight,
};

enum class Test {
	Deploy,
	LowSpeed,
	HighSpeed,
	Flight,
};

// range [0,4095], outside range is undefined behavior
enum class Speed : uint16_t {
	Off = 0,
	LowSpeed = 50,
	Flight = 2400,
	HighSpeed = 2537,
	MaxSpeed = HighSpeed,
};

static Speed operator ""_rpm(unsigned long long i)
{
	assert(i <= static_cast<int>(Speed::MaxSpeed));

	// Does not work: must be explicitly initialized to Speed.
	//
	// return i;

	// Does not work: narrowing type is not allowed (unsigned long long
	// should be narrowed to uint16_t).
	//
	// return Speed{i};

	return Speed{static_cast<std::underlying_type_t<Speed>>(i)};
}

// Operator overloading of enums
Speed& operator<<(Speed& speed, Test& test)
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
	Mode mode = Reset;
	Speed speed = Speed::Off;

	Test test{Test::Deploy};
	mode = Mode::Idle;

	test = Test::LowSpeed;
	mode = PreFlight;
	speed << test;

	// implicit conversion
	int i = mode;
	mode = (Mode)(i + 1);
	std::cout << "mode = " << mode << std::endl;
	// Asking for troubles...

	speed = 1000_rpm;
	// No implicit conversion to integral value.
//	std::cout << "speed = " << speed << std::endl;
	std::cout << "speed = " << static_cast<int>(speed) << std::endl;

	// ambiguous?
	mode = Flight;

	// Strongly typed, but this is still allowed.
	test = (Test)3;
	test = static_cast<Test>(3);
	test = Test{3};

	// Mode should be fixed, enum class would be better.
	// Firmware update required.
	mode = (Mode)0;

	// enum class is nice because of the scoping, but normal enums are
	// still useful, for example for macro-like definitions:

	enum Ingenuity {
		Height_cm = 49,
		Diameter_cm = 120,
		Mass_g = 1800,
		SoftwareVersion = 2,
	};

	return SoftwareVersion;
}

