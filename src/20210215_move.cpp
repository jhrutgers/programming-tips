/*
 * Move sematics
 *
 */

// std::move is in here.
#include <utility>

#include <iostream>
#include <set>
#include <list>
#include <string>

typedef std::string Stuff;

class Country {
public:
	Country(char const* name) : m_name(name) {}

	// By declaring a move constructor and/or assignment operator, the
	// implicit copy constructor and assignment operator is deleted.
	Country(Country&& country) {
		*this = std::move(country);
	}

	Country& operator=(Country&& country) {
		m_name = std::move(country.m_name);
		m_laws = std::move(country.m_laws);
		m_treaties.clear();
		country.m_treaties.clear();
		return *this;
	}

	std::string const& name() const { return m_name; }

//	void trade(Stuff stuff)        { std::cout << "Trade by copy" << std::endl; }
	void trade(Stuff const& stuff) { std::cout << "Trade by const reference" << std::endl; }
	void trade(Stuff& stuff)       { std::cout << "Trade by reference" << std::endl; }
	void trade(Stuff&& stuff)      { std::cout << "Trade by rvalue reference" << std::endl; }

	// Required to put a Country in an std::set.
	bool operator<(Country const& other) const { return name() < other.name(); }
private:
	std::string m_name;
	std::list<Stuff> m_laws;
	std::list<Stuff> m_treaties;
};

int main()
{
	// First a few notes on lvalues and rvalues.

	//          lvalue     = rvalue;
	std::string referendum = "Brexit";

	// Bad move.
	//          lvalue     = lvalue;
	std::string cameron    = referendum;

	//          lvalue     = std::move(lvalue);
	std::string may        = std::move(cameron);
	// cameron is now in an undefined state.
	(void)may; // Suppress warning about usefulness.


	// More complex example.
	Country uk("UK");

	std::set<Country> eu;
	// Note: std::unsorted_set has constant time complexity for insertion
	// and removal. It would probably be better, but we need a hash
	// function for Country, which is tricky to do right. So, leave it for
	// now.

	eu.emplace(std::move(uk));

#if __cplusplus >= 201703L
	// Remove the node from the std::set.
	auto uk_node = eu.extract("UK");
	// Move the value from the node to the independent uk variable.
	uk = std::move(uk_node.value());
#else
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

	// Overloads get a bit harder to get right.

	// By copy or reference
	Stuff shares;
	uk.trade(shares);

	// Move temporary
	uk.trade(Stuff("knowledge"));

	// Explicit move
	Stuff goods;
	uk.trade(std::move(goods));

	// Const reference.
	Stuff const annoyance;
	uk.trade(annoyance);
}

