/*
 * Lambda functions
 *
 * Ever needed a small function that is only used in one place, such as a
 * comparator function?  This is where lambda functions come in. Lambda(
 * function)s are anonymous functions, with a compact syntax, so you can create
 * one easily. Moreover, it is easy to pass local variables to that lambda.
 *
 * You will see why that is great. Scroll down to main() and start reading from
 * there.  You may want to build and run the application, as it will give you
 * interesting results.
 */

// Lambdas are part of C++; no headers have to be included. These are just for
// this example.
#include <vector>
#include <utility>
#include <limits>
#include <iostream>
#include <algorithm>
#include <cmath>
#include <numeric>
#include <functional>
#include <random>
#include <string>

// Let's (type)def a few types.
using Year = int;
using Globally_average_temperature_offset_from_some_reference = double;
using Sample = std::pair<Year,Globally_average_temperature_offset_from_some_reference>;

// Provide an overload of the stream operator in order to use std::cout.
std::ostream& operator<<(std::ostream& os, Sample const& sample) {
	return os << sample.first << ": " << sample.second;
}

// Some data to work with.
static std::vector<Sample> const gistemp_v4 {
	{1880, -0.16}, {1881, -0.07}, {1882, -0.10}, {1883, -0.16}, {1884, -0.28},
	{1885, -0.32}, {1886, -0.31}, {1887, -0.35}, {1888, -0.17}, {1889, -0.10},
	{1890, -0.35}, {1891, -0.22}, {1892, -0.27}, {1893, -0.31}, {1894, -0.30},
	{1895, -0.22}, {1896, -0.11}, {1897, -0.11}, {1898, -0.26}, {1899, -0.17},
	{1900, -0.07}, {1901, -0.15}, {1902, -0.27}, {1903, -0.36}, {1904, -0.46},
	{1905, -0.26}, {1906, -0.22}, {1907, -0.38}, {1908, -0.42}, {1909, -0.48},
	{1910, -0.43}, {1911, -0.43}, {1912, -0.35}, {1913, -0.34}, {1914, -0.15},
	{1915, -0.13}, {1916, -0.35}, {1917, -0.45}, {1918, -0.29}, {1919, -0.27},
	{1920, -0.27}, {1921, -0.18}, {1922, -0.28}, {1923, -0.26}, {1924, -0.27},
	{1925, -0.22}, {1926, -0.10}, {1927, -0.21}, {1928, -0.20}, {1929, -0.36},
	{1930, -0.16}, {1931, -0.09}, {1932, -0.16}, {1933, -0.29}, {1934, -0.12},
	{1935, -0.20}, {1936, -0.15}, {1937, -0.03}, {1938,  0.00}, {1939, -0.02},
	{1940,  0.13}, {1941,  0.18}, {1942,  0.07}, {1943,  0.09}, {1944,  0.20},
	{1945,  0.09}, {1946, -0.07}, {1947, -0.03}, {1948, -0.11}, {1949, -0.11},
	{1950, -0.17}, {1951, -0.07}, {1952,  0.01}, {1953,  0.08}, {1954, -0.13},
	{1955, -0.14}, {1956, -0.19}, {1957,  0.05}, {1958,  0.06}, {1959,  0.03},
	{1960, -0.03}, {1961,  0.06}, {1962,  0.03}, {1963,  0.05}, {1964, -0.20},
	{1965, -0.11}, {1966, -0.06}, {1967, -0.02}, {1968, -0.08}, {1969,  0.05},
	{1970,  0.03}, {1971, -0.08}, {1972,  0.01}, {1973,  0.16}, {1974, -0.07},
	{1975, -0.01}, {1976, -0.10}, {1977,  0.18}, {1978,  0.07}, {1979,  0.16},
	{1980,  0.26}, {1981,  0.32}, {1982,  0.14}, {1983,  0.31}, {1984,  0.16},
	{1985,  0.12}, {1986,  0.18}, {1987,  0.32}, {1988,  0.39}, {1989,  0.27},
	{1990,  0.45}, {1991,  0.41}, {1992,  0.22}, {1993,  0.23}, {1994,  0.32},
	{1995,  0.45}, {1996,  0.33}, {1997,  0.46}, {1998,  0.61}, {1999,  0.38},
	{2000,  0.39}, {2001,  0.54}, {2002,  0.63}, {2003,  0.62}, {2004,  0.54},
	{2005,  0.68}, {2006,  0.64}, {2007,  0.67}, {2008,  0.55}, {2009,  0.66},
	{2010,  0.72}, {2011,  0.61}, {2012,  0.65}, {2013,  0.68}, {2014,  0.75},
	{2015,  0.90}, {2016,  1.02}, {2017,  0.93}, {2018,  0.85}, {2019,  0.99},
	{2020,  1.02}
};

// Use a lambda as callback. F&& is a universal reference; it fits to any
// lvalue/rvalue type of thing.  We are going to call it as a function with one
// argument, but that is not expressed in the template or argument list. So, if
// you make an error, you will get some compile error at the line where f is
// used.
//
// F can be a real function pointer, a lambda, or a functor. It does not
// matter, as long as we can apply an argument to it.
template <typename C, typename F>
static void print(C&& collection, F&& f) {
	std::cout << "[ ";
	for(auto const& item : collection)
		std::cout << f(item) << " ";
	std::cout << "]" << std::endl;
}

// This is a way to save a reference to one external variable (Capture),
// and expect one argument (A) to be passed to the ()-operator.
template <typename Capture, typename A>
struct NaiveLambda {
	// This is the function pointer to call when (*this)(a) is called.
	void(*f)(Capture, A);
	// The other variable to be passed to f().
	Capture capture;

	// The ()-operator is called when the following piece of code is
	// executed:
	//
	// NaiveLambda l;
	// A a;
	// l(a); //  <--- here
	//
	// In this case, forward the argument, and the capture to the function.
	// Now, the capture variable is contained in the instance of NaiveLambda,
	// and this instance behaves like a function; it is a functor.
	void operator()(A a) const { f(capture, a); }
};

static void find_coldest_f(Sample& prev, Sample x)
{
	if(x.second < prev.second)
		prev = x;
}

int main(int argc, char** argv) {
	// Consider the following problem: we want to know the year with the
	// lowest temperature. std::for_each() can iterate over a list, but
	// takes a function-like argument, which is used to pass every value
	// through. There are other ways to implement this search, but let's
	// stick to std::for_each() for now. To build this function, we have to
	// do the following.
	//
	// This is the result variable, which is to be set by that function.
	Sample coldest = { 0, std::numeric_limits<double>::infinity() };

	// A function cannot access this by default, so we have to pass it as a
	// reference. As std::for_each() expects a function with only one
	// argument, we use a functor; a class with the ()-operator
	// implemented. Check the implementation of NaiveLambda above.
	NaiveLambda<Sample&,Sample> find_coldest { find_coldest_f, coldest };

	// Find and print the value we were looking for.
	std::for_each(gistemp_v4.begin(), gistemp_v4.end(), find_coldest);
	std::cout << "Globally coldest year: " << coldest << std::endl;


	// Ok, that worked, but was a lot of work. However, this is basically
	// what a lambda can do for you, but with any number and type of
	// capture variables and arguments. So, let's do the same, but now for
	// the hottest year.
	Sample hottest { 0, -std::numeric_limits<double>::infinity() };
	std::for_each(gistemp_v4.begin(), gistemp_v4.end(),
		[&](Sample const& x) {
			if(x.second > hottest.second)
				hottest = x;
		});
	// The lambda definition has three parts:
	//   [...]   the capture list;
	//   (...)   the argument list; and
	//   {...}   the implementation.
	//
	// The argument list and implementation is what you already know for
	// any other normal function. The capture list can be:
	// - [&]   Capture all by reference; which means that every variable
	//         used in the implementation that is not a variable or local
	//         variable, should be brought in from the external scope as a
	//         reference. In this case, hottest is captured.
	// - [=]   Capture all by value; the same as above, but now make a
	//         local copy of the variables, instead of using references.
	// - []    Do not capture anything.
	// - Explicit list of variables, like [&hottest] to capture hottest as
	//         reference or [hottest] to capture it by value.  The list is
	//         comma separated and can be mixed. E.g., [a,&b] captures a by
	//         value and b by reference, [&,a] captures a by value and all
	//         others by reference.
	//
	// This is just syntactic sugar for the NaiveLambda with static
	// function as show above, but now in a compact notation.

	std::cout
		<< "Globally hottest year: "
		<< hottest
		<< " (multiplicity: "
		<< std::count_if(gistemp_v4.begin(), gistemp_v4.end(),
			[&](Sample const& x) -> bool { return x.second == hottest.second; })
		<< ")" << std::endl;
	// std::count_if iterates over a list and returns the number of items
	// for which the function argument returns true. In this case, it
	// counts the number of years with the same temperature. Note the '->
	// bool' part, which is a trailing return type. This explicitly
	// specifies that the lambda returns a bool. If omitted, the return
	// type is deducted from the return statement. However, it might be
	// worth to explicitly specify it, if deduction would be ambiguous
	// (like if you want to return a value or a reference).
	//
	// If you are a beginner on lambda functions, you should not need it,
	// so omit it.  If you need it, you are not a beginner anymore...

	// Lets duplicate the data, and sort it. std::sort() expects a function
	// with two arguments, as a comparator function. In this case, the
	// capture list is empty, as there is no external variable to be
	// brought in from the current scope. You could have defined a static
	// function too, but this is a very compact way, as the comparator is
	// only used here. So, there is no need to pollute the global scope for
	// that.
	auto gistemp_v4_sorted { gistemp_v4 };
	std::sort(gistemp_v4_sorted.begin(), gistemp_v4_sorted.end(),
		[](Sample const& a, Sample const& b) { return a.second < b.second; });

	// In this case, we define a function ourselves that accept the lambda
	// as callback.
	std::cout << "Years sorted by average global temperature: ";
	print(gistemp_v4_sorted, [](Sample const& x) { return x.first; });

	// There is no way to make a templated lambda (till C++20), as you
	// could do with a normal function.  However, if you specify auto as
	// argument type, that effectively becomes a template.  So, in this
	// example, the lambda accept any type of argument, as if 'auto' was a
	// template type.
	//
	// We are going to do some curve fitting in a moment. For that, we need
	// to compute some root-mean-square value, where f() takes a year and
	// returns the temperature, which is compared to our data set. Try to
	// understand how it works.
	auto rms = [&](auto&& f) {
		double e = std::accumulate(gistemp_v4.begin(), gistemp_v4.end(), 0.0,
			[&](double acc, Sample const& sample) {
				return acc + std::pow(sample.second - f(sample.first), 2); }); // square
		e /= (double)gistemp_v4.size(); // mean
		return std::sqrt(e); // root
	};
	// Hmm, such a large lambda is not very comfortable. Reducing the
	// lambda and increase insulation is probably the best advice in
	// general, as it reduces the complexity of the actual problem we are
	// trying to solve...

	// The variable rms is a lambda function, but it is unspecified what
	// the type will be. If you want to assign a lambda to a variable, use
	// an std::function. Any function pointer or lambda can be saved in it.
	// When calling f(2021.16) it passes the argument 2021.16 to the
	// function saved in the std::function.  If no function is saved (like
	// it is after using the default constructor), calling it does nothing.
	std::function<double(double)> f;

	double err = std::numeric_limits<double>::infinity();

	// Fun with lambdas: this is a lambda returning a lambda. Note the
	// capture by value, as the returned lambda outlives the arguments a,
	// b, c, and d, so the cannot be captured by reference.
	auto generator = [](double a, double b, double c, double d) {
		return [=](double x) { return a * std::pow(b, x + c) + d; }; };

	// Find a function f(x) = a*b^(x+c)+d, such that rms(f) is minimized
	// for a, b, c, and d.  Ok, there are smarter ways (just solving
	// mathematically or using gradient descent), but this one is easy to
	// implement... And using some more computing power once in a while
	// won't hurt anyone, right?
	//
	// If you are in a hurry to get back to work, you may skip over this
	// implementation part, as it only computes a function that should
	// change your life, but who cares. The truth is that what it says is
	// inconvenient anyway.
	//
	// By the way, that generator generates an exponential function. And
	// there was something tricky with that kind of functions, especially
	// in the long run. Or wait, no, in the not-so-long run...
	std::random_device rd;
	std::minstd_rand0 random(rd());
	std::uniform_real_distribution<> a_dis(0, 10.0);
	std::uniform_real_distribution<> b_dis(1.0, 1.1);
	std::uniform_real_distribution<> c_dis(-2100.0, -1900.0);
	std::uniform_real_distribution<> d_dis(-1.0, 1.0);

	unsigned long iterations = 1000000;
	try {
		if(argc > 1)
			iterations = std::max(1ul, std::stoul(argv[1]));
	} catch(...) {
	}

	// In case you missed it, you can pass the number of iterations as the
	// first command line argument of this program.

	for(unsigned long i = 0; i < iterations; i++) {
		// I'm feeling lucky...
		double a = a_dis(random);
		double b = b_dis(random);
		double c = c_dis(random);
		double d = d_dis(random);

		// Get a function.
		auto f_ = generator(a, b, c, d);

		// Check if is better than we already had.
		double err_ = rms(f_);
		if(err_ < err) {
			std::cout << i << ": f(x) = "
				<< a << " * " << b << "^(x + " << c << ") + " << d
				<< " (RMS " << err_ << ")" << std::endl;

			// Save it.
			f = std::move(f_);
			err = err_;
		}
	}

	// Now, f is set to the function with the lowest RMS value, so the best
	// function that resembles the data.

	std::cout << "Average global temperature above normal according to Paris Agreement: " << std::endl;
	auto preindustrial = f(1880);
	for(auto y : { 1990, 2000, 2010, 2020, 2030, 2040, 2050, 2060, 2070 })
		std::cout << y << ": " << f(y) - preindustrial << std::endl;

	// Enable the next lines if you want to see the data in Excel.
#if 0
	std::cout << "CSV dump of measured data and curve fitted data:" << std::endl;
	for(auto const& sample : gistemp_v4)
		std::cout << sample.first << "," << sample.second << "," << f(sample.first) << std::endl;
#endif

	// It is left as an exercise to the reader to determine in what year
	// the two agreed temperature limits are broken.



	// A few final notes. Lambdas can do everything normal functions can.
	// However, I would suggest that you only use a lambda when the
	// implementation is simple (one or a few lines of code), and it is
	// only used in one place (like a comparator function as shown above).
	// If the capture list is longer than than all-by-reference or
	// all-by-value, it might be a hint that you are trying to solve a
	// complex issue, and lambdas do not make that simpler.

	return u'λ';
}

/*
 * Further reading:
 *
 * https://en.cppreference.com/w/cpp/language/lambda
 * https://en.cppreference.com/w/cpp/algorithm
 * https://en.cppreference.com/w/cpp/utility/functional/function
 */
