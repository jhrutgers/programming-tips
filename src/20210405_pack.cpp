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
struct Container;

template <typename C>
struct is_container { constexpr static bool value = false; };

template <typename... T>
struct is_container<Container<T...>> { constexpr static bool value = true; };

template <typename... C>
struct has_container {};

template <>
struct has_container<> { constexpr static bool value = false; };

template <typename C0, typename... C>
struct has_container<C0, C...> { constexpr static bool value = is_container<std::decay_t<C0>>::value || has_container<C...>::value; };

template <typename... C>
inline constexpr bool has_container_v = has_container<C...>::value;


template <typename... T>
struct Container {
	template <typename... S,
		std::enable_if_t<!has_container_v<S...>, bool> = true>
	constexpr Container(S&&... stuff)
		: cargo(std::forward<S>(stuff)...)
	{}

	explicit constexpr Container(std::tuple<T...> const& stuff)
		: cargo(stuff)
	{}

	constexpr static size_t size()
	{
		return sizeof...(T);
	}

	std::tuple<T...> cargo;

	template <size_t... Indices>
	void inspect(std::index_sequence<Indices...> indices = std::index_sequence<Indices...>()) const
	{
		((std::cout << std::get<Indices>(cargo) << " "), ...);
		if(sizeof...(Indices))
			std::cout << std::endl;
	}

	void inspect() const
	{
		inspect(std::make_index_sequence<size()>());
	}

	template <typename... C,
		std::enable_if_t<!has_container_v<C...>, bool> = true>
	constexpr auto add(C&&... more_stuff) const
	{
		return Container<T..., std::decay_t<C>...>(std::tuple_cat(cargo, std::make_tuple(more_stuff...)));
	}

	template <typename... C>
	constexpr auto add(Container<C...> const& container) const
	{
		return Container<T..., C...>(std::tuple_cat(cargo, container.cargo));
	}

	template <typename C>
	constexpr auto operator+(C&& thing) const
	{
		return add(std::forward<C>(thing));
	}
};


// https://stackoverflow.com/questions/43430921/deduction-guides-and-variadic-class-templates-with-variadic-template-constructor
template <typename A0, typename... A>
Container(A0&& stuff0, A&&... stuff) -> Container<std::decay_t<A0>, std::decay_t<A>...>;

int main()
{
	Container<int, double> c{2,3.4};
	c.inspect();
	c.inspect<1,0,0,1>();
	std::string s("b");
	auto c2 = Container(3.1f, std::string("a"), s, "c");
	c2.inspect();
	auto c3 = c2.add(true, std::string("abc"));
	c3.inspect();

	auto c4 = c3 + 12 + 1.3f;
	c4.inspect();
	auto c5 = c2.add(3, 1) + c4;
	c5.inspect();

	Container<int,double> cc = c;
	(void)cc;
	std::tuple ever_green{c, c2, Container(4)};
	std::apply([](auto&&... container) { (container.inspect(), ...);} , ever_green);
}

/*
 * Further reading:
 *
 * https://en.cppreference.com/w/cpp/language/parameter_pack
 * https://en.cppreference.com/w/cpp/utility/integer_sequence
 */

