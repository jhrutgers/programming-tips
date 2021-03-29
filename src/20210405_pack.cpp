/*
 * Parameter packs
 *
 */

#include <cstddef>
#include <iostream>
#include <tuple>
#include <type_traits>
#include <utility>

template <typename... T>
struct Container {
	Container(T&&... stuff)
		: contents(std::forward<T>(stuff)...)
	{}

	Container(std::tuple<std::decay_t<T>...> const& stuff)
		: contents(stuff)
	{}

	constexpr static size_t size()
	{
		return sizeof...(T);
	}

	std::tuple<std::decay_t<T>...> contents;

	template <size_t... Indices>
	void dump(std::index_sequence<Indices...> indices = std::index_sequence<Indices...>()) const
	{
		((std::cout << std::get<Indices>(contents) << " "), ...);
		if(sizeof...(Indices))
			std::cout << std::endl;
	}

	void dump() const
	{
		dump(std::make_index_sequence<size()>());
	}

	template <typename... C>
	auto add(C&&... more_stuff)
	{
		return Container<T..., std::decay_t<C>...>(std::tuple_cat(contents, std::make_tuple(more_stuff...)));
	}
};

template <typename... A>
Container(A&&... stuff) -> Container<A...>;//std::decay_t<A>...>;

int main()
{
	Container<int, double> c{2,3.4};
	c.dump();
	c.dump<1,0,0,1>();
	std::string s("b");
	auto c2 = Container(3.1f, std::string("a"), s, "c");
	c2.dump();
	auto c3 = c2.add(true, std::string("abc"));
	c3.dump();

}

/*
 * Further reading:
 *
 * https://en.cppreference.com/w/cpp/language/parameter_pack
 * https://en.cppreference.com/w/cpp/utility/integer_sequence
 */

