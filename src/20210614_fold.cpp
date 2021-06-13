/*
 * Fold expressions
 *
 * We have seen the fold expression a few times already, as it comes in very
 * handy when you are processing parameters packs. Let's see in more detail
 * what it is about.
 */

#include <iostream>
#include <tuple>
#include <utility>

struct Headache {
	constexpr static double severity{0.1};
};

template <typename Tuple>
static auto accumulate([[maybe_unused]] Tuple&& tuple, std::index_sequence<>)
{
	return 0;
}

template <typename Tuple, size_t I0, size_t... I>
static auto accumulate(Tuple&& tuple, std::index_sequence<I0, I...>)
{
	return std::get<I0>(tuple) + accumulate(tuple, std::index_sequence<I...>());
}

template <typename... Arg>
static auto f(Arg&&... arg)
{
	// Complex way of doing a fold with a tuple and index_sequence (C++14).
	// For C++11, you could implement the index_sequence yourself, but that
	// makes it even tricker.  This is not something I would recommend.
	std::tuple t{arg...};
	return accumulate(t, std::make_index_sequence<sizeof...(Arg)>());
}

template <typename... Arg>
static auto f2(Arg&&... arg)
{
	// Same as f, but way easier.
	// left fold: (((arg0 + arg1) + arg2) + arg3)...
	return (... + arg);
}

template <typename... Arg>
static auto f3(Arg&&... arg)
{
	// right fold: (arg0 + (arg1 + (arg2 + (arg3...))))
	return (arg + ...);
}

template <typename... Arg>
static auto f4(Arg&&... arg)
{
	// left fold with init: ((((I + arg0) ... + arg1) + arg2) + arg3)...
	return (1 + ... + arg);
}

template <typename... Arg>
static auto f5(Arg&&... arg)
{
	// right fold with init: (arg0 + (arg1 + (arg2 + ... (arg3 + I)))))
	return (arg * ... * 2);
}

int main()
{
	std::cout << f(0, 1, 2.3) << std::endl;
	std::cout << f2(0, 1, 2.3) << std::endl;
}

/*
 * Further reading:
 *
 * https://en.cppreference.com/w/cpp/language/fold
 * https://en.wikipedia.org/wiki/Fold_(higher-order_function)
 */

