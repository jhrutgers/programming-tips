/*
 * Move semantics
 *
 * C++ is always concerned about performance. Since C++11, you can act on when
 * you know an object is going out of scope.  Most commonly, this is used with
 * constructor/function parameters. As a result, you can make a difference as
 * receiving function whether the parameter can be harvested for its resources,
 * or a (deep) copy must be made.
 *
 * Read on starting from main() for a few examples.
 */

// std::move is in here.
#include <utility>

#include <iostream>
#include <set>
#include <list>
#include <string>
#include <memory>
#include <functional>

typedef std::string Stuff;

class Country {
public:
	Country(char const* name) : m_name(name) {}

	// By declaring a move constructor and/or assignment operator, the
	// implicit copy constructor and assignment operator is deleted.
	// However, it is better to be explicit and follow the "Rule of five"
	// (either implement destructor, copy constructor, copy assignment,
	// move constructor and move assignment, or implement none of them).

	~Country() = default;
	Country(Country const&) = delete;
	void operator=(Country const&) = delete;

	// The move constructor. Typically implemented as a move assignment.
	// Note of the argument type that distinguishes between copy and move.
	Country(Country&& country) {
		*this = std::move(country);
	}

	// By default, the move assignment operator is implemented, which is a
	// std::move of all members. If you want different behavior, override
	// it. Note the double &, which is an rvalue reference.
	Country& operator=(Country&& country) {
		m_name = std::move(country.m_name);
		m_laws = std::move(country.m_laws);
		m_treaties.clear();
		country.m_treaties.clear();
		return *this;
	}

	std::string const& name() const { return m_name; }

	// Specify different overloads with different value categories.
	//
	// Never do call by value, as it duplicates the object. Always do by
	// reference, which is cheaper (except for primitive types like int and
	// float, but the compiler can sort that out).
//	void trade(Stuff people)          { std::cout << "Trade by copy" << std::endl; }

	// Reference to a (non-const) lvalue. You may modify the object, but it
	// will outlive the function call.
	void trade(Stuff& services)       { std::cout << "Trade by reference" << std::endl; }

	// Reference to a const lvalue. You can use the object, but you cannot
	// modify it, as it is const.
	void trade(Stuff const& capital)  { std::cout << "Trade by const reference" << std::endl; }

	// rvalue reference. If this overload is called, you know that you may
	// destroy/harvest the object, as it is thrown away afterwards. You may
	// do the same as with the lvalue reference overload, but you can also
	// do something more efficient.
	void trade(Stuff&& goods)         { std::cout << "Trade by rvalue reference" << std::endl; }

	// Required to put a Country in an std::set.
	bool operator<(Country const& other) const { return name() < other.name(); }

private:
	std::string m_name;
	std::list<Stuff> m_laws;
	std::list<Stuff> m_treaties;
};

static std::string brexit() {
	return "Brexit";
}

int main()
{
	// First a few notes on lvalues and rvalues.  Every expression C++ is
	// from some type. The taxonomy is quite elaborate, but is it probably
	// enough to understand the difference between lvalue and rvalue.  An
	// lvalue is something that has a memory address, like a variable. An
	// rvalue does not have an address, like the result of an expression.
	// Think of lvalue/rvalue as the _l_eft-hand side and _r_ight-hand side
	// of an assignment.

	//     lvalue = rvalue; a number has no address.
	double remain = 48.1;
	//     lvalue = rvalue; as it is the result of an expression.
	double leave  = 100 - remain;
	//     lvalue = lvalue; you can assign (read) from an lvalue.
	double result = leave;
	(void)result; // Suppress warning; ignore exact numbers.


	// It gets more interesting when objects are involved.

	//          lvalue     = rvalue; the result of the call is a temporary.
	std::string referendum = brexit();
	// A temporary has no address. Note what happens here. An std::string
	// has an internal buffer that saves the actual string.  As the
	// compiler knows that the returned value will be out of scope at the
	// end of the statement, it would be a waste to first allocate a buffer
	// in referendum, copy the contents from the temporary, and then
	// release the buffer from the temporary.  It is better to _move_ the
	// buffer from the temporary to referendum. That is almost for free
	// (just swapping a pointer).  Here you have got the move semantics;
	// move the content from a 'dying' object into another one.

	// Bad move.
	//          lvalue     = lvalue;
	std::string cameron    = referendum;
	// As the referendum variable is not out of scope here, it is now
	// unsafe to move its contents because of the assignment, as it would
	// destroy the referendum. So, in this case, the std::string's buffer
	// is duplicated into cameron, leaving the referendum in tact.

	// But, what if we want to forcibly move it? Call std::move(), which
	// takes an lvalue, and makes an rvalue from it.
	//          lvalue     = rvalue;
	std::string may        = std::move(cameron);
	// cameron is now in an undefined state.

	// Don't do this, it's really awkward. It is undefined behavior.
	may = std::move(may);

	(void)may; // Suppress warning about usefulness.


	// More complex example. Country implements move semantics; it has a
	// constructor and assignment operator that implement the move.
	Country uk("UK");

	std::set<Country> eu;
	// Note: std::unsorted_set has constant time complexity for insertion
	// and removal, while std::set is logarithmic. It would probably be
	// better to use std::unsorted_set instead, but we need a hash function
	// for Country, which is tricky to do right. So, leave it for now.

	// There are multiple ways to get into the set:
	eu.insert(Country("Netherlands")); // By rvalue reference (move in place).
	eu.emplace("Belgium"); // By construct in-place.
	eu.emplace(Country("France")); // By move-construct in-place.
	eu.emplace(std::move(uk)); // By move-construct in-place.

	// This would duplicate the Country first, which is a waste. Moreover,
	// its copy constructor was deleted.
//	Country cyprus("Cyprus");
//	eu.insert(cyprus);

	// Assume, you want to move things back out, without copy/duplication.
#if __cplusplus >= 201703L
	// From C++17 on, it is easier to move stuff out of containers.
	// Remove the node from the std::set.
	auto uk_node = eu.extract("UK");
	// Move the value from the node to the independent uk variable.
	uk = std::move(uk_node.value());
#else
	// This is equivalent, but it was a bit nastier in the old days.
	// Retrieve an iterator to where "UK" is in the set.
	auto it = eu.find("UK");
	if(it != eu.end()) {
		// Move it from the set into the independent uk variable,
		// but this leaves an empty Country in the set...
		uk = std::move(*it);
		// ...which should be removed.
		eu.remove(it);
	}
#endif

	// As you can see, the std::set API allows different ways to pass stuff
	// around.  To show the different ways of calling a function, watch
	// what happens when we call different overloads of trade().  They all
	// 'just work', but the compiler picks a different overload.

	// Call by reference.
	Stuff lse;
	uk.trade(lse);

	// Const reference.
	Stuff const stubbornness;
	uk.trade(stubbornness);

	// Temporary implies rvalue reference.
	uk.trade(Stuff("food"));

	// By using std::move(), you kind of permit the other party of
	// consuming it; this explicit move results in a rvalue reference.
	Stuff vaccins;
	uk.trade(std::move(vaccins));

	// As you can see, it is up to the designer of the API to do it
	// properly.  But then, it is easy to use. The STL containers are full
	// of these kinds of tricks, which may be hard(er) to read the API, but
	// in the end makes the library efficient.



	// You may wondering now, why not using pointers to pass an object
	// around, without the complexity of value types and moves? It is
	// possible, such as:
	std::list<std::reference_wrapper<Country>> cptpp;
	auto global_uk = std::make_shared<Country>("Global Britain");
	cptpp.push_back(*global_uk);

	auto machinery = std::make_unique<Stuff>();
	for(auto& country : cptpp)
		country.get().trade(*machinery);

	// However, using the cptpp is not as nice, as the user of the API must
	// do some extra work to get the pointer handling right (and think of
	// ownership, smart pointers, and dangling pointers), but it is mostly
	// a matter of taste.  Choose what works for you and makes the
	// interface intuitive. At least you now have the option of the move
	// semantics.
}

/*
 * Further reading:
 *
 * https://en.cppreference.com/w/cpp/utility/move
 * https://en.cppreference.com/w/cpp/language/move_constructor
 * https://en.cppreference.com/w/cpp/language/value_category
 * https://en.cppreference.com/w/cpp/language/rule_of_three
 */

