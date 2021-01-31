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
#include <memory>
#include <functional>

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

//	void trade(Stuff people)          { std::cout << "Trade by copy" << std::endl; }
	void trade(Stuff const& capital)  { std::cout << "Trade by const reference" << std::endl; }
	void trade(Stuff& services)       { std::cout << "Trade by reference" << std::endl; }
	void trade(Stuff&& goods)         { std::cout << "Trade by rvalue reference" << std::endl; }

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

	//          lvalue     = rvalue;
	std::string may        = std::move(cameron);
	// cameron is now in an undefined state.

	// Don't do this, it's really awkward. It is undefined behavior.
	may = std::move(may);

	(void)may; // Suppress warning about usefulness.


	// More complex example.
	Country uk("UK");

	std::set<Country> eu;
	// Note: std::unsorted_set has constant time complexity for insertion
	// and removal, while std::set is logarithmic. It would probably be
	// better to use std::unserted_set instead, but we need a hash function
	// for Country, which is tricky to do right. So, leave it for now.

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

	// By copy (not implemented) or reference
	Stuff lse;
	uk.trade(lse);

	// Const reference.
	Stuff const stubbornness;
	uk.trade(stubbornness);

	// Move temporary
	uk.trade(Stuff("whiskey"));

	// Explicit move
	Stuff flowers;
	uk.trade(std::move(flowers));




	// Why not using pointers to pass around?
	std::list<std::reference_wrapper<Country>> cptpp;
	auto global_uk = std::make_shared<Country>("Global Britain");
	cptpp.push_back(*global_uk);

	auto machinery = std::make_unique<Stuff>();
	for(auto& country : cptpp)
		country.get().trade(*machinery);
}

