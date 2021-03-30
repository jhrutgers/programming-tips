/*
 * Parameter packs
 *
 * You know templates, right? So, std::set has a template parameter, that
 * specifies what the type within the set it. And std::tuple can have multiple
 * of them.  Well, std::tuple can have any number of template parameters... We
 * dive into this topic: parameter packs.
 *
 * Parameter packs exist since C++11, but along the way, some syntax changed to
 * make using them a bit easier. This example uses C++17.
 *
 * Follow the sequence of navigation marks in this file. Make sure to follow
 * the right sequence, or you may get stuck. Start with {alpha}.
 */

// Parameter packs are part of the language. These are just for this example.
#include <cstddef>
#include <ios>
#include <iostream>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>

// Forward declare the Container.
template <typename... T>
class Container;

// {juliett}
// We are diving deep into this stuff. You may want to skip it, but I will
// briefly indicate what happens here.
//
// This is a struct that checks if C is a Container type, indicated by its
// value. By default, nothing is a Container.
template <typename C>
struct is_container { constexpr static bool value = false; };

// This specialization of is_container does the detection in case any Container
// type was passed to is_container.
template <typename... T>
struct is_container<Container<T...>> { constexpr static bool value = true; };

// This struct checks if any type with in the pack C is a Container.  By
// default, it returns that no types are found. However, this default
// implementation is only used in case no types are passed to has_container, as
// for one or more types, there is a specialization below.
template <typename... C>
struct has_container { constexpr static bool value = false; };

// This specialization passes the first type through is_container, and
// recursively calls itself to check for the rest.
template <typename C0, typename... C>
struct has_container<C0, C...> {
	constexpr static bool value =
		is_container<std::decay_t<C0>>::value ||
		has_container<C...>::value;
};

// Helper to make using has_container a bit simpler. This is variable template.
template <typename... C>
inline constexpr bool has_container_v = has_container<C...>::value;
// Continue to the next marker.


// {bravo}
// A class with one template type would be:
// template <typename T> class Container;
//
// One with three different types would be:
// template <typename T0, typename T1, typename T2> class Container;
//
// One with arbitrary number of templates would be:
template <typename... T>
class Container {
// Right, typename... stands for any number of template types.  They are
// combined in the template parameter pack, named T.  As it is not a single
// type, but a pack of types, they are used a bit different.
public:

	// For example, to get the number of types within the pack, use the
	// sizeof... operator. This is has nothing to do with the size of the
	// types in bytes, what the normal sizeof would return.
	//
	// The sizeof... is static, so this function can be static. Moreover,
	// it is constexpr. We will discuss that later on.
	constexpr static size_t size()
	{
		return sizeof...(T);
	}

	// The Container is to hold sizeof...(T) values. In this case, we use
	// an std::tuple for that. The tuple expects a list of types to be
	// contained.  For this, we give it our parameter pack T. However, we
	// can't pass the pack as is, we have to unpack it:
	std::tuple<T...> cargo;
	// Here, T... is unpacked resulting in the elements of the pack,
	// separated by commas. So, in case of Container<int,double>,
	// std::tuple<T...> will be unpacked to std::tuple<int,double>.

	// Parameter packs can be used in different places.  Here we define a
	// constructor that accepts any number and type of parameters.
	template <typename... S,
		// Ignore this line for now. We will come back to that.
		std::enable_if_t<!has_container_v<S...>, bool> = true>
	// To construct the list of arguments, S is unpacked. Specifically, the
	// type S&& is unpacked. So if you would pass an int& and double const*
	// parameter, it is unpacked to int&&& and double const*&&. The double
	// & denotes a universal reference; it will accept all types of
	// references, such as lvalues and std::move()d objects. This is a
	// common way of defining such generic list of arguments.
	constexpr Container(S&&... stuff)
		// In this case, we forward all arguments as is to the
		// constructor of our tuple. We use std::forward() to preserve
		// the exact type of the argument, including move semantics --
		// this is perfect forwarding. Without it, we might copy data
		// more often than required.  But we have the parameter pack S
		// in it.  The full expression in which the pack is used, is
		// unpacked before forwarding to cargo.
		: cargo{std::forward<S>(stuff)...}
	{}
	// Navigate to the next marker.

	explicit constexpr Container(std::tuple<T...> const& stuff)
		: cargo(stuff)
	{}

	// {echo}
	// This function accepts any number of size_t template values.
	template <size_t... Indices>
	// The indices parameter isn't actually used, only for template
	// deduction.  When you specify an index_sequence (like inspect() does,
	// Indices is deduced.  When you specify Indices, the indices parameter
	// can be omitted. In any case, the Indices pack holds a sequence of
	// size_t numbers.
	void inspect(std::index_sequence<Indices...> indices = std::index_sequence<Indices...>()) const
	{
		// Almost there, we have to unpack Indices and pass the values
		// to std::get. For this, we use a fold expression. There are
		// multiple forms, but the one we use here is this:
		//
		// ( expr operator ... )
		//
		// expr may hold a pack. For example, for a pack of three
		// elements, this expression is unfolded to:
		//
		// ( expr_0 operator expr_1 operator expr_2 )
		//
		// The comma is used as operator. It does not do anything,
		// except for separating expressions. See how Indices is
		// unpacked, and std::get is now called for every element in
		// the pack.
		((std::cout << std::get<Indices>(cargo) << " "), ...);

		// Print a newline if the pack has a size larger than 0.
		if(sizeof...(Indices))
			std::cout << std::endl;

		// Go to the next mark.
	}

	void inspect() const
	{
		// {delta}
		// We would like to iterate over all elements in the tuple and
		// print them.  However, the only way to get access to tuple
		// elements is by using std::get<i>(cargo), where i is a
		// constant expression. So this won't work:
		//
		// for(int i = 0; i < size(); i++)
		//	std::cout << std::get<i>(cargo);
		//
		// In this case, i is dynamic, and not computed at compile
		// time.  What we are going to do, is to create the sequence
		// [0,1,2,...,size()-1] as template values, and use unpack
		// magic to print them. The standard library helps us with
		// these sequences.  For example, std::make_index_sequence<4>
		// gives us the type std:index_sequence<0,1,2,3>. So, this
		// expands to the indices of the tuple. That's a start.

		inspect(std::make_index_sequence<size()>());
		//                               ^^^^^^
		// Hey, what happens here -------++++++
		// A function call as template parameter? Indeed. Template
		// parameters must be a compile time constant expression, but
		// size() was defined as such an expression; it is constexpr.
		// There are a few details to know about constexpr, but that's
		// for later.
		//
		// Continue to the next mark.
	}

	// {golf}
	// Again, a function with a parameter pack to allow any number of
	// arguments, specified as universal reference. Continue to the next
	// marker.
	template <typename... S,
		// {india}
		// When this expression with templates...
		//               vvvvvvvvvvvvvvvvvvvvvv
		std::enable_if_t<!has_container_v<S...>, bool> = true>
		//                                       ^^^^-----------------+
		// ...evaluates to true, the type of the enable_if_t will be  | .
		// Otherwise, a compile error occurs, and this function
		// overload will be discarded by the compiler while looking
		// which add() overload is to be called.  This is called
		// Substitution Failure Is Not An Error (SFINAE); you won't get
		// a compile error when that happens. Continue to the next mark.
	constexpr auto add(S&&... more_stuff) const
	{
		// {hotel}
		// This expression says:
		return
			// Construct a Container with all our types, and the
			// types within S (where decay_t removes const/volatile
			// and references from the types).
			Container<T..., std::decay_t<S>...>{
				// Initialize the container by concatenating
				// two tuples: our current cargo, and a tuple
				// created from all arguments we got passed.
				std::tuple_cat(
					cargo,
					std::make_tuple(std::forward<S>(more_stuff)...)
				)
			};
	}

	// The add() above should not put Containers in Containers.  This
	// overload allows to add the contents of another container to our one.
	template <typename... C>
	constexpr auto add(Container<C...> const& container) const
	{
		return Container<T..., C...>(std::tuple_cat(cargo, container.cargo));
	}
	// However, the typename... S from the other add() captures everything,
	// including Containers. The compiler might pick the wrong overload
	// when adding a Container.  Here, the std::enable_if_t thing comes
	// into play, which was also used with the constructor.  Continue to
	// the next marker.

	template <typename A>
	constexpr auto operator+(A&& thing) const
	{
		return add(std::forward<A>(thing));
	}
};


// If you got lost, this line is a Class Template Argument Deduction guide.  It
// says: when Container's constructor is called without specifying its template
// arguments explicitly, deduct it as follows: decay all types. So, if
// Container(int&, double volatile) would be called, the actual Container type
// will be Container<int,double>, and not Container<int&,double volatile>, what
// it is by default.
//
// Wondering what A0 is doing there? Don't ask... Bugs...
// https://stackoverflow.com/questions/43430921/deduction-guides-and-variadic-class-templates-with-variadic-template-constructor
template <typename A0, typename... A>
Container(A0&& stuff0, A&&... stuff) -> Container<std::decay_t<A0>, std::decay_t<A>...>;

int main()
{
	// {alpha}
	// Consider the case that you want to have some Container that holds
	// arbitrary objects. This is how you want to use it:
	Container<int, double, std::string> sometimes_red{1, 2.3, "4"};

	// This Container now holds three objects, of the types specified as
	// template arguments, and initialized using its constructor.
	// Another container could hold only one big value:
	auto probably_blue = Container{5e67L};

	// The template type of Container is now deducted as Container<long double>,
	// which will be filled in for auto.

	// Now, the question would be how to create such a class? Continue with
	// the next marker: {bravo}.

	// {charlie}
	// Now we have Containers, we can inspect them. We will define a
	// function that prints the contents to stdout. Go to the next marker
	// for details.
	std::cout << "sometimes red: ";
	sometimes_red.inspect();

	// {foxtrot}
	// We can manually specify the sequence, and it does not have to be an
	// increasing one:
	Container often_purple{"o", "t"};
	std::cout << "often purple: ";
	often_purple.inspect<1,0,0,1>();

	// Containers should be full. Assume we want to add stuff to it.
	auto rarely_black = probably_blue.add("more", true, "value");
	std::cout << "rarely black: " << std::boolalpha;
	rarely_black.inspect();
	// Continue to the next marker to check out add() works.

	// {kilo}
	// Now, we can add even containers:
	auto commonly_white = sometimes_red.add(often_purple);
	std::cout << "commonly white: ";
	commonly_white.inspect();

	// We could also use the overloaded + operator:
	auto frequently_yellow = commonly_white + "Intel" + 3 + "ARM";
	std::cout << "frequently yellow: ";
	frequently_yellow.inspect();

	// If we keep packing, it might become huge:
	std::tuple ever_given{commonly_white, often_purple, Container("ever", "green")};
	// By now, you should be able for figure out this one by yourselves. If
	// not, continue to the next marker.
	std::apply([](auto&&... container) { (container.inspect(), ...);} , ever_given);
}

/*
 * Further reading:
 *
 * https://en.cppreference.com/w/cpp/language/parameter_pack
 * https://en.cppreference.com/w/cpp/utility/integer_sequence
 * https://en.cppreference.com/w/cpp/language/variable_template
 * https://en.cppreference.com/w/cpp/language/class_template_argument_deduction
 */

