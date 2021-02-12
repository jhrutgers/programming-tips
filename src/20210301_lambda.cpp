/*
 * Lambda functions
 *
 */

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

using Year = int;
using Globally_average_temperature_offset_from_some_reference = double;
using Sample = std::pair<Year,Globally_average_temperature_offset_from_some_reference>;

std::ostream& operator<<(std::ostream& os, Sample const& sample) {
	return os << sample.first << ": " << sample.second;
}

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

// Use lambda as callback.
template <typename C, typename F>
static void print(C&& collection, F&& f) {
	std::cout << "[ ";
	for(auto const& item : collection)
		std::cout << f(item) << " ";
	std::cout << "]" << std::endl;
}


template <typename Capture, typename A>
struct NaiveLambda {
	void(*f)(Capture, A);
	Capture capture;

	void operator()(A a) const { f(capture, a); }
};

static void find_coldest_f(Sample& prev, Sample x)
{
	if(x.second < prev.second)
		prev = x;
}

int main(int argc, char** argv) {
	// Own implementation of lambda function.
	Sample coldest = { 0, std::numeric_limits<double>::infinity() };
	NaiveLambda<Sample&,Sample> find_coldest { find_coldest_f, coldest };

	for(auto const& sample : gistemp_v4)
		find_coldest(sample);

	std::cout << "Globally coldest year: " << coldest << std::endl;


	// Use lambda function.
	Sample hottest { 0, -std::numeric_limits<double>::infinity() };
	std::for_each(gistemp_v4.begin(), gistemp_v4.end(),
		[&](Sample const& x) {
			if(x.second > hottest.second)
				hottest = x;
		});

	// Lambdas are compact.
	std::cout
		<< "Globally hottest year: "
		<< hottest
		<< " (multiplicity: "
		<< std::count_if(gistemp_v4.begin(), gistemp_v4.end(),
			[&](Sample const& x) -> bool { return x.second == hottest.second; })
		<< ")" << std::endl;

	// Without capture, it is just a static function.
	auto gistemp_v4_sorted { gistemp_v4 };
	std::sort(gistemp_v4_sorted.begin(), gistemp_v4_sorted.end(),
		[](Sample const& a, Sample const& b) { return a.second < b.second; });

	// Use as callback.
	std::cout << "Years sorted by average global temperature: ";
	print(gistemp_v4_sorted, [](Sample const& x) { return x.first; });

	// Use as variable, with auto (=template) argument.
	auto rms = [&](auto&& f) {
		double e = std::accumulate(gistemp_v4.begin(), gistemp_v4.end(), 0.0,
			[&](double acc, Sample const& sample) {
				return acc + std::pow(sample.second - f(sample.first), 2); });
		return std::sqrt(e / (double)gistemp_v4.size());
	};

	// Save (and reset) a lambda
	std::function<double(double)> f;
	double err = std::numeric_limits<double>::infinity();

	// A lambda returning a lambda
	auto generator = [](double a, double b, double c, double d) {
		return [=](double x) { return a * std::pow(b, x + c) + d; }; };

	// Find a function f(x) = a*b^(x+c)+d, such that rms(f(x)) is minimized for a, b, c, and d.
	// OK, there are smarter ways, but this one is easy to implement...
	std::random_device rd;
	std::minstd_rand0 gen(rd());
	std::uniform_real_distribution<> a_dis(0, 10.0);
	std::uniform_real_distribution<> b_dis(1.0, 1.1);
	std::uniform_real_distribution<> c_dis(-1900.0, -2100.0);
	std::uniform_real_distribution<> d_dis(-1.0, 1.0);

	unsigned long iterations = 1000000;
	try {
		if(argc > 1)
			iterations = std::max(1ul, std::stoul(argv[1]));
	} catch(...) {
	}

	for(unsigned long i = 0; i < iterations; i++) {
		double a = a_dis(gen);
		double b = b_dis(gen);
		double c = c_dis(gen);
		double d = d_dis(gen);
		auto f_ = generator(a, b, c, d);
		double err_ = rms(f_);
		if(err_ < err) {
			std::cout << i << ": f(x) = "
				<< a << " * " << b << "^(x + " << c << ") + " << d
				<< " (RMS " << err_ << ")" << std::endl;
			f = std::move(f_);
			err = err_;
		}
	}

	std::cout << "Average global temperature above normal according to Paris Agreement: " << std::endl;
	auto preindustrial = f(1880);
	for(auto y : { 1990, 2000, 2010, 2020, 2030, 2040, 2050, 2060, 2070 })
		std::cout << y << ": " << f(y) - preindustrial << std::endl;

	// Uncomment next line if you want to see the data in Excel.
#if 0
	std::cout << "CSV dump of measured data and curve fitted data:" << std::endl;
	for(auto const& sample : gistemp_v4)
		std::cout << sample.first << "," << sample.second << "," << f(sample.first) << std::endl;
#endif
}

