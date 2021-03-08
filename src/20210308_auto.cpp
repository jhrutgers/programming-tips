/*!
 * auto
 *
 * In cases where the compiler can deduct a type, you don't have to specify it;
 * just use 'auto' as a type. It makes the code shorter, but not necessarily
 * easier to understand... Let's investigate when it is a good idea to use
 * auto.
 *
 * Just work your way down through this file. There is no need to run it,
 * although compiling it makes sure that all the static_assert()s check out.
 */

// auto is a keyword, no includes are required; these are just for this example.
#include <iostream>
#include <string>
#include <map>
#include <utility>
#include <memory>
#include <type_traits>
#include <cassert>

int main()
{
	std::cout << "C++ version is " << __cplusplus << std::endl;

#if __cplusplus < 201103L // Until C++03
	{
		// Before C++11, auto was a storage class specifier (like
		// static/extern/thread_local). It means: automatic storage
		// duration.  In fact, by default every variable was 'auto',
		// unless specified differently (like 'static'). As it is the
		// default, the keyword was practically never used.  But still,
		// code that was perfectly valid in 2003, just breaks later
		// on...
		static int spirit = 20040104;
		auto int opportunity = 20040125;
	}

	// spirit remains static (but not accessible in this scope), but
	// opportunity is out of scope here, and discarded properly here, as it
	// is auto.

	// But, forget about the past, let's move on.
#endif

	// Here, the auto keyword is used as a type, which is deducted from the
	// initializer.  In this example, the type is int. I'm not sure if this
	// is a great usage, as you may want to have an int32_t or int64_t
	// instead to have explicit value ranges.
	auto launch = 20200730;
	static_assert(std::is_same<decltype(launch), int>::value, "");
	// About this static_assert, is uses the decltype specifier. This is a
	// bit like auto as it determines the type, but differently; decltype
	// returns the actual type of the argument, with reference/
	// const/volatile.

	// In this case, the type is deducted from the result of the
	// expression, which is still an int.
	auto arrive = launch + 203;
	static_assert(std::is_same<decltype(arrive), int>::value, "");

	// Note that auto is the base type, without reference/const/volatile.
	// So, here auto is still deducted as int. If you would need an int&,
	// use auto&. This usage of auto is great; we just follow the type of
	// arrival, whatever it is.
	auto& perseverance = arrive;
	static_assert(std::is_same<decltype(perseverance), int&>::value, "");
	perseverance = 20210218;
	assert(arrive == 20210218);

	// Again, if it has to be const or volatile, specify it:
	auto volatile& ingenuity = arrive;
	static_assert(std::is_same<decltype(ingenuity), int volatile&>::value, "");
	assert(ingenuity == 20210218);

	std::string message = "Hello Mars!\n";

	// This is a great usage of auto: iterators. Instead of writing:
	//
	// for(std::string::iterator_type it = message.begin() ...
	//
	// you only have to write:
	for(auto it = message.begin(); it != message.end(); ++it)
		std::cout << *it;
	// auto here is deducted as the iterator type, as that is what begin()
	// returns.

	// This is also great. You don't need to know the exact type of what
	// the container has, but you know that you want to use it as a const
	// reference.
	for(auto const& x : message) {
		std::cout << x;
		static_assert(std::is_same<decltype(x), char const&>::value, "");
	}

	// This saves a lot of typing too. Instead of writing down the type of
	// the map twice, you only have to do it once, while readability
	// improves:
	//
	// std::map<int, std::string>* jezero = new std::map<int, std::string>(...);
	auto jezero = new std::map<int, std::string>({/* ... */});
	delete jezero; // "Who needs a map if you got lasers..."

	// Similarly, it is crystal clear to what auto will be deducted.
	//
	// std::shared_ptr<int> moxie = std::make_shared(02);
	auto moxie = std::make_shared<int>(02);

	// std::string* mastcam_z = new std::string(...);
	auto mastcam_z = new std::string("forest");
	delete mastcam_z; // "Nah, don't need it, we are looking for dust and rocks..."

	// This is questionable. 'new auto' is resolved based on the
	// initializer. In this case, you may guess wrong what the type would
	// be... So, don't.
	auto rimfax = new auto("oil");
	// This is probably not what you expect.
	static_assert(std::is_same<decltype(rimfax), char const**>::value, "");
	delete rimfax; // "Nah, don't need it, we are looking for water..."

	// This is great. The type of a lambda is unspecified, so it is not
	// possible to explicitly write out the type of the variable that holds
	// the lambda.
	auto drill = [](int sample) { return sample; };

	// Question: what is the return type of drill? If you don't specify it,
	// it is auto, which is exactly the same as the type of collect; int:
	auto collect = [](int sample) -> auto { return sample; };
	// Explicitly specifying the return type as auto is not really useful.

	// You may explicitly specify it to make sure that the sample is not
	// duplicated, but left intact. In this case, the return type could
	// also be auto&, which is equivalent.
	auto seal = [](int& sample) -> int& { return sample; };

#if __cplusplus >= 201402L
	// In contrast to normal functions, you cannot have templated lambdas
	// (until C++20).  Since C++14, there is a way, namely to use auto as
	// parameter type. This becomes a generic lambda, which is like if the
	// argument is deducted like a template type would be. Question is,
	// what will the function do?  I would recommend to avoid this, but it
	// may be useful sometimes...
	auto store = [](auto&& j) {};

	// In case you really don't know the type upfront, using 'auto' may be
	// tricky as the reference is dropped. Here is an alternative:
	// decltype(auto). Is has a similar purpose as 'auto', but it is
	// different... Similar to auto, the type is deducted from the
	// initializer, int in this case.
	decltype(auto) samples = 43;
	static_assert(std::is_same<decltype(samples), int>::value, "");

	// Here, the type will be the type of the expression; int.
	decltype(auto) fetch = samples + 5;
	static_assert(std::is_same<decltype(fetch), int>::value, "");

	// Now, it becomes tricky; decltype() of an identifier returns the
	// exact type of that identifier (int in this case).
	decltype(auto) mav = fetch;
	static_assert(std::is_same<decltype(mav), int>::value, "");

	// However, decltype() of an expression may return an lvalue in this
	// case.  So, ero becomes an int&, while 'auto ero = (mav)' would be an
	// int...
	decltype(auto) ero = (mav);
	static_assert(std::is_same<decltype(ero), int&>::value, "");

	// rvalue references are deducted too...
	decltype(auto) earth = std::move(ero);
	static_assert(std::is_same<decltype(earth), int&&>::value, "");

	// decltype(auto) can also be used as return type. In this case,
	// it forces the return type of the lambda to int&.
	auto deposit = [](int& sample) -> decltype(auto) { return sample; };
	decltype(auto) cache1 = deposit(samples);
	static_assert(std::is_same<decltype(cache1), int&>::value, "");

	// Compare this to using auto:
	auto cache2 = deposit(samples);
	static_assert(std::is_same<decltype(cache2), int>::value, "");

	// But this really complicates understanding. Can you predict the type
	// of cache1 and cache2? Probably not, so it is better just to be
	// explicit and declare cache1 and cache2 explicitly as int& or int
	// instead.
#endif
}

#if __cplusplus >= 201402L
// Similar to lambdas, you can use auto as return type of a function.  However,
// I strongly recommend to document the function properly, as this completely
// complicates meaning.
auto answer()
{
	return 42;
}

// Using decltype(auto) might be tempting in this case. Here, the operator+
// might be overloaded, in case you cannot predict what the type would be, as
// you don't know the template parameter types. So, returning auto may solve
// the issue.  However, no programmer will understand what you will get without
// reading the source (and even with reading it). So, you probably shouldn't do
// this.
template <typename Liquid, typename Chemical>
decltype(auto) habitable(Liquid&& water, Chemical&& building_blocks)
{
	return water + building_blocks;
}

// Analog to lambdas, you can have trailing return type. There are a few
// reasons to do this, but I am not going to discuss them (now). However, it
// makes the function declaration even more complex, so avoid it if you don't
// have a good reason.
template <typename Air>
auto convert(Air&& co2) -> decltype(auto)
{
	return co2 -= 'C';
}
#endif

#if __cplusplus >= 202002L
#include <ranges>

// In C++20, the auto parameter type (as already seen with the lambda) can also
// be used for normal functions. This is the abbreviated function template,
// which is equivalent to:
// template <typename T> auto sherloc(T&& uv) { ... }
auto sherloc(auto&& uv)
{
	return std::ranges::data(uv);
}
#endif

// So, in general, use auto or decltype(auto) to save typing (in case of long
// types), but only if one can still determine the type easily from the rest of
// the line, for example. This will make your program easier to understand.
//
// However, these tools are great if the type cannot be determined upfront,
// like the templated functions above. But use it wisely.

/*
 * Further reading:
 *
 * Definition of auto and decltype(auto):
 * https://en.cppreference.com/w/cpp/language/auto
 *
 * It's tough to read how return type deduction works, so use with care:
 * https://en.cppreference.com/w/cpp/language/function#Return_type_deduction
 *
 * Check out the trailing return type of lambdas:
 * https://en.cppreference.com/w/cpp/language/lambda
 */

