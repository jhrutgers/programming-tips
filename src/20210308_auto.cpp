/*!
 * auto
 */

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
		// storage class specifier (like static/extern/thread_local)
		// automatic storage duration, valid till 2003.
		static int spirit = 20040104;
		auto int opportunity = 20040125;
	}

	// spirit remains static (but not accessible in the current scope),
	// opportunity is out of scope here.
#endif

	auto launch = 20200730;
	static_assert(std::is_same<decltype(launch), int>::value, "");

	auto arrive = launch + 203;
	static_assert(std::is_same<decltype(arrive), int>::value, "");

	auto& perseverance = arrive;
	static_assert(std::is_same<decltype(perseverance), int&>::value, "");
	perseverance = 20210218;
	assert(arrive == 20210218);

	auto volatile& ingenuity = arrive;
	static_assert(std::is_same<decltype(ingenuity), int volatile&>::value, "");
	assert(ingenuity == 20210218);

	std::string message = "Hello Mars!\n";
	// for(std::string::iterator_type it = message.begin() ...
	for(auto it = message.begin(); it != message.end(); ++it)
		std::cout << *it;

	for(auto const& x : message) {
		std::cout << x;
		static_assert(std::is_same<decltype(x), char const&>::value, "");
	}

	// std::map<int, std::string> jezero = new std::map<int, std::string>(...);
	auto jezero = new std::map<int, std::string>({/* ... */});
	delete jezero;

	// std::shared_ptr<int> moxie = std::make_shared(02);
	auto moxie = std::make_shared<int>(02);

	// std::string* mastcam_z = new std::string(...);
	auto mastcam_z = new std::string("forest");
	delete mastcam_z; // "Nah, don't need it, we are looking for dust and rocks..."

	auto rimfax = new auto("oil");
	// This is probably not what you expect.
	static_assert(std::is_same<decltype(rimfax), char const**>::value, "");
	delete rimfax; // "Nah, don't need it, we are looking for water..."

	auto drill = [](int sample) { return sample; };
	auto collect = [](int sample) -> auto { return sample; };
	auto seal = [](int& sample) -> int& { return sample; };

#if __cplusplus >= 201402L
	// generic lambda
	auto store = [](auto&& j) {};

	decltype(auto) samples = 43;
	static_assert(std::is_same<decltype(samples), int>::value, "");

	decltype(auto) fetch = samples + 5;
	static_assert(std::is_same<decltype(fetch), int>::value, "");

	decltype(auto) mav = fetch;
	static_assert(std::is_same<decltype(mav), int>::value, "");

	decltype(auto) ero = (mav);
	static_assert(std::is_same<decltype(ero), int&>::value, "");

	decltype(auto) earth = std::move(ero);
	static_assert(std::is_same<decltype(earth), int&&>::value, "");

	auto deposit = [](int& sample) -> decltype(auto) { return sample; };
	decltype(auto) cache1 = deposit(samples);
	static_assert(std::is_same<decltype(cache1), int&>::value, "");
	auto cache2 = deposit(samples);
	static_assert(std::is_same<decltype(cache2), int>::value, "");
#endif
}

#if __cplusplus >= 201402L
auto answer()
{
	return 42;
}

template <typename Liquid, typename Chemical>
auto habitable(Liquid&& water, Chemical&& building_blocks)
{
	return water + building_blocks;
}

template <typename Air>
auto convert(Air&& co2) -> decltype(auto)
{
	return co2 -= 'C';
}
#endif

#if __cplusplus >= 202002L
#include <ranges>

// abbreviated function template
auto sherloc(auto uv)
{
	return std::ranges::data(uv);
}
#endif

