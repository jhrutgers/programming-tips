/*
 * Fold expressions
 *
 * We have seen the fold expression a few times already, as it comes in very
 * handy when you are processing parameters packs. Let's see in more detail
 * what it is about.
 *
 * Scroll down to main, and check out the parameter packs in the program flow.
 */

// Fold expressions are part of the language. These are just for this example.
#include <iostream>
#include <tuple>
#include <type_traits>
#include <utility>

// Let's define arbitrary types.
struct DeadColleague       { constexpr static double severity{1000.0}; };
struct MissingLeg          { constexpr static double severity{500.0}; };
struct BrokenMaterial      { constexpr static double severity{200.0}; };
struct BrokenShoulderBlade { constexpr static double severity{300.0}; };
struct BleedingHead        { constexpr static double severity{100.0}; };
struct Concussion          { constexpr static double severity{200.0}; };
struct Vomiting            { constexpr static double severity{50.0}; };
struct HitByDrone          { constexpr static double severity{100.0}; };
struct FallFromStage       { constexpr static double severity{10.0}; };
struct BumpedToe           { constexpr static double severity{1.0}; };
struct Emotional           { constexpr static double severity{2.0}; };
struct BadHairDay          { constexpr static double severity{0.5}; };

// Recursive accumulate() terminator.
template <typename Tuple>
static auto accumulate([[maybe_unused]] Tuple&& tuple, std::index_sequence<>)
{
	return 0;
}

// Recursively process the index sequence, folding the element of the first
// index to the rest of the sequence.
template <typename Tuple, size_t I0, size_t... I>
static auto accumulate(Tuple&& tuple, std::index_sequence<I0, I...>)
{
	return std::get<I0>(tuple) + accumulate(tuple, std::index_sequence<I...>());
}

// This function is to be summing all arguments.  Although the types of the
// arguments can be anything -- it's a parameter pack -- it should have
// something in common, such as that it supports the operator+, or it has a
// severity member, in this case.
template <typename... Arg>
static auto sum(Arg&&... arg)
{
	// You cannot access the arg pack by index directly, so in order to
	// iterate, you would probably assign it to a tuple.
	std::tuple t{arg.severity...};

	// Now, you can access the tuple by index. You may be tempted to do
	// something like:
	//
	// double sum = 0;
	// for(size_t i = 0; i < sizeof...(Arg); ++i)
	//	sum += std::get<i>(t);
	//
	// However, i is not a constexpr, so it cannot be passed as template
	// parameter to std::get. And a tuple cannot be accessed by a value
	// only known at run-time. (Think for a while about why this is the
	// case.) For this, you could use an std::index_sequence (since C++14).
	// An index_sequence holds a series of size_t as template parameters.
	// So, this will generate std::index_sequence<0, 1, 2, 3, etc.>.  Using
	// recursion, we can process all elements of the tuple at compile-time.
	return accumulate(t, std::make_index_sequence<sizeof...(Arg)>());

	// However, this is very tricky and non-intuitive. As it makes your
	// code hard to understand, avoid it.  C++17 has a better alternative,
	// the fold expression.
}

// Let's try it again.
template <typename... Arg>
static auto add(Arg&&... arg)
{
	// This result is the same as sum(), but way easier.
	//
	// This is a left fold. It is expanded to:
	// (((arg0 + arg1) + arg2) + arg3)...
	//
	// So, it folds (recursively) all values to combine it into a single
	// result.
	return (... + arg.severity);

	// The surrounding braces are mandatory. The position of the pack
	// expansion and the operator should be this way, the expression
	// following the operator can be anything, as long as it is based on
	// the parameter pack.
}

template <typename... Arg>
static auto any(Arg&&... arg)
{
	// Note the reversed order of the expression and the pack expansion.
	// This is the right fold, which expands to:
	// (arg0 * (arg1 * (arg2 * (arg3...))))
	//
	// The operator can be any operator, including assignment and
	// comparison. In this case, all values are multiplied. Fair enough, it
	// does not really matter if this particular fold is left or right, as
	// multiplication is (mathematically) associative. For assignment or
	// division, it is relevant, though.
	return (arg.severity * ...);
}

template <typename A, typename B>
static auto operator^(A&& a, B&& b)
{
	using T = std::common_type_t<A, decltype(std::declval<B>().severity)>;
	return std::max<T>(static_cast<T>(a), static_cast<T>(b.severity));
}

template <typename... Arg>
static auto tough_guy(Arg&&... arg)
{
	// The fold expression only works with operators. If you want to do
	// custom operations, like finding the std::max() of arbitrary types,
	// you would have to overload some operator...
	//
	// Note the initial value in the fold expression. This is a left fold,
	// which would expand to:
	// ((((init ^ arg0) ^ arg1) ^ arg2) ^ arg3)...
	return (1u ^ ... ^ arg);

	// Quite ugly to misuse an operator for it, but I can imagine that
	// there are valid use cases.
}

template <typename... Arg>
static auto who_earns_the_most(Arg&&... arg)
{
	// You may guess the form of the final fold expression variant:
	// A right fold with initializer, expanded to:
	// (arg0, (arg1, (arg2, ... (arg3, init)))))
	//
	// Using the comma operator is a way to call arbitrary functions at the
	// left side of the operator, as its return type is ignored; the result
	// of operator, is the right side.
	return ((std::cout << arg), ..., 0);

	// Note the additional braces around the expression. If you would write:
	// return (std::cout << arg, ..., 0);
	//
	// ...the compiler gets confused what the intended fold operator is (<<
	// or ,).
}

int main()
{
	std::cout << "For reason X, this professional completes his job:" << std::boolalpha << std::endl;

	// There are four ways of doing a fold.
	std::cout << "Soldier: "            << (sum(DeadColleague{}, MissingLeg{}, BrokenMaterial{})     < std::numeric_limits<double>::infinity()) << std::endl;
	std::cout << "Pro cyclist: "        << (add(BrokenShoulderBlade{}, BleedingHead{}, Concussion{}) < 1'000'000) << std::endl;
	std::cout << "Performer on stage: " << (any(Vomiting{}, HitByDrone{}, FallFromStage{})           <   100'000) << std::endl;
	std::cout << "Soccer player: "      << (tough_guy(BumpedToe{}, Emotional{}, BadHairDay{})        <         1) << std::endl;

	return who_earns_the_most("Mind"," over"," matter", "...", "\n");
}

/*
 * Further reading:
 *
 * https://en.cppreference.com/w/cpp/language/fold
 * https://en.wikipedia.org/wiki/Fold_(higher-order_function)
 */

