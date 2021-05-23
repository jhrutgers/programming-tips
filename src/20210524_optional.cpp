/*
 * std::optional
 *
 * When a function may or may not return a value, C++17 provides std::optional,
 * which expresses this either-the-function-was-successful-or-there-is-no-data-
 * to-be-returned behavior.
 *
 * Let's see how it works. Read from top to bottom.
 */

// This header is required for std::optional. The others are just for this
// example.
#include <optional>

#include <any>
#include <chrono>
#include <exception>
#include <functional>
#include <iostream>
#include <memory>
#include <random>
#include <string>
#include <tuple>
#include <utility>
#include <variant>

// If you have a function that may or may not return data, you can do that in
// multiple ways. For example:

// For C programmers: this actually means: srand(time(NULL))
static std::default_random_engine generator(
	static_cast<unsigned>(std::chrono::system_clock::now().time_since_epoch().count()));

// For C programmers: toss() wraps a call like: rand() % 2
static std::uniform_int_distribution<int> distribution(0,1);
static auto toss = std::bind(distribution, generator);

using namespace std::literals;

static bool f(std::string& x)
{
	if(!toss())
		return false;

	x = "asdf";
	return true;
}

static std::unique_ptr<std::string> k()
{
	return toss() ? std::make_unique<std::string>("asdf") : nullptr;
}

static std::pair<std::string, bool> g()
{
	return toss() ?
		std::make_pair("asdf"s, true) :
		std::make_pair(""s, false);
}

static std::tuple<std::string, bool> i()
{
	return toss() ?
		std::make_tuple("asdf"s, true) :
		std::make_tuple(""s, false);
}

static std::variant<std::monostate, std::string> h()
{
	std::variant<std::monostate, std::string> res;
	return toss() ? res = "asdf"s : res;
}

static std::any j()
{
	return toss() ? std::any{"asdf"s} : std::any{};
}

static std::string l()
{
	if(toss())
		return "asdf"s;

	throw "Truth bomb";
//	throw std::exception("asdf");
//	throw no_data();
}

static std::optional<std::string> m()
{
	std::optional<std::string> res;
	if(toss())
		res = "asdf"s;

	return res;
}

static std::optional<std::string> n()
{
	if(toss())
		return "asdf"s;

	return std::nullopt;
}

int main()
{
	if(std::string x; f(x))
		std::cout << "arg: " << x.c_str() << std::endl;

	// pointer data
	if(auto x = k())
		std::cout << "unique_ptr: " << x->c_str() << std::endl;

	if(auto x = g(); x.second)
		std::cout << "pair: " << x.first.c_str() << std::endl;

	if(auto x = i(); std::get<bool>(x))
		std::cout << "tuple: " << std::get<std::string>(x).c_str() << std::endl;

	if(auto x = h(); auto p = std::get_if<std::string>(&x))
		std::cout << "variant: " << p->c_str() << std::endl;

	if(auto x = j(); auto p = std::any_cast<std::string>(&x))
		std::cout << "any: " << p->c_str() << std::endl;

	try {
		auto x = l();
		std::cout << "exceptions: " << x.c_str() << std::endl;
	} catch(...) {
	}

	if(auto x = m(); x.has_value())
		std::cout << "optional 1: " << x.value().c_str() << std::endl;

	if(auto x = n())
		std::cout << "optional 2: " << x->c_str() << std::endl;
}

