/*
 * Brace initialization
 *
 * C++11 introduces a uniform way to initialize values and aggregates, and make
 * this more robust, compared to other ways of initialization.
 *
 * This program shows what you can do with it. Scroll down to main() and follow
 * the execution flow. For understanding the brace initialization, you don't
 * need to run the program.
 */

// Brace initialization is part of the language; no includes are required.
// These are just for this example.
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <string>

// This is a simple struct, no user-defined constructor.
struct Bank {
	std::string name;
};

// Same kind of content as the Bank struct above, but now with constructors.
class Investor {
public:
	// In this case, m_name is constructed according to its in-class
	// initialization.
	Investor() = default;

	explicit Investor(std::string const& name)
		// Check this out: the member initialization also uses {}
		// instead of ().
		: m_name{name}
	{}

	explicit Investor(std::string&& name) noexcept
		: m_name{std::move(name)}
	{}

	std::string const& name() const { return m_name; }

private:
	// Pass "anonymous" to std::string's constructor.  It is only used in
	// case of the default constructor, as the other constructors provide a
	// different initialization.
	std::string const m_name{"anonymous"};
};

// Other example structs, where in-class member initializers are provided,
// which are used when not explicitly specified during construction.
struct Company {
	// This will create an std::string with contents "???".
	// You could also write something like:
	//
	//   std::string const name = std::string("???");
	//
	// However, that will call the copy or move constructor of std::string.
	// So, this won't work if the class doesn't support copying/moving.
	std::string const name{"???"};

	// An empty initializer value-initializes the object. In this case, it
	// defaults to 0.
	double value{};
};

struct Share {
	// References must be initialized. As there is no initializer specified
	// here, one must provide it while constructing a Share.
	Company const& company;
	Bank const& owner;
	double amount{};

	double value() const { return company.value * amount; }
};

// Some more complex class.
class Contract {
public:
	Contract(Bank const& bank, Investor const& investor, Company const& company, double value, double rate)
		: m_bank{bank}
		, m_investor{investor}
		, m_share{company, bank, value / company.value}
		, m_rate{rate}
		, m_initialValue{m_share.value()}
	{}

	double totalReturn() const      { return m_share.value() - m_initialValue; }
	double profitToInvestor() const { return totalReturn() - profitToBank(); }
	double profitToBank() const     { return m_rate * m_initialValue; }

	// On a side note, this is not a member function of Contract... It's a
	// friend, declared in the same scope as Contract is.
	friend std::ostream& operator<<(std::ostream& stream, Contract const& contract)
	{
		stream.imbue(std::locale("en_US.UTF-8"));

		auto result = contract.m_share.value() / contract.m_initialValue - 1.0;
		auto profit = contract.profitToInvestor();
		return stream
			<< std::showbase
			<< "Share of " << contract.m_share.company.name
			<< " changed value by " << result * 100.0 << "% to " << std::put_money(contract.m_share.value() * 100.0) << "; "
			<< contract.m_bank.name << " earns " << std::put_money(contract.profitToBank() * 100.0) << ", "
			<< contract.m_investor.name() << (profit >= 0 ? " earns " : " loses ") << std::put_money(std::abs(profit * 100.0))
			<< std::endl;
	}

private:
	Bank const& m_bank;
	Investor const& m_investor;
	Share m_share;
	double m_rate{};
	double m_initialValue{};
};

int main()
{
	// Initializing structs/classes are done in several ways.  For example,
	// Bank does not have a constructor, so you can initialize its fields:
	Bank cs = {"Credit Suisse"};
	// As a draw-back, you need to know the structure of the data type.

	// If you have a constructor, you can do this:
	Investor james("Simons");
	// However, you need to know that Investor has a constructor.

	// If you want to call the default constructor, you may be tempted to
	// write this:
	Investor anon();
	// However, this is the most vexing parse; this is not an instance of
	// Investor, it is a function declaration that takes no arguments and
	// returns an Investor instance.

	// In C++11, this is solved by defining braced initialization. This is
	// a uniform way to initialize.  It works on structs, where you would
	// initialize the fields:
	Bank nmr{"Nomura"};
	Bank gs{"Goldman Sachs"};
	Company viac{"ViacomCBS", 27'000'000'000.};

	// It also works on constructors, where the () is just replaced by {}:
	Investor bill{"Hwang"};

	// Now, calling the default constructor does what you expect.
	Investor anon2{};


	// Let's play some more.

	std::cout << "Year 1" << std::endl;
	// Example: this is a contract between a bank (cs) and an investor
	// (bill). They agree that the total return of a share (viac) is
	// swapped; it the share goes up, the benefit is for the investor, if
	// the share goes down, the loss is for the investor. The investor pays
	// a fixed rate (1%) over the value ($1,000,000) of the contract.
	//
	// The bank owns the share within this contract. The bank just gets the
	// rate.
	Contract contract1{cs, bill, viac, 1'000'000., 0.01};

	// Assume the share goes up.
	viac.value *= 1.05; // 5 % growth

	// As long as the share goes up more than the rate of the contract, you
	// don't need money for the contract; it only gives you profit.
	std::cout << contract1;
	// Great, got some money by hard work! Let's do that again.

	std::cout << "Year 2" << std::endl;
	Contract contract2{nmr, bill, viac, 10'000'000., 0.02};
	Contract contract3{gs, bill, viac, 100'000'000., 0.01};
	viac.value *= 1.04; // 3 % growth
	std::cout << contract1 << contract2 << contract3;
	// Whoo, free money... As the rate is lower than the growth, the
	// contracts are for free! Let's do that again!

	std::cout << "Year 3" << std::endl;

	// The brace initializer can also be used for arrays.
	Contract contracts[]{
		contract1, contract2, contract3,
		contract1, contract2, contract3,
		contract1, contract2, contract3,
		contract1, contract2, contract3,
		// ...
		contract1, contract2, contract3,
		contract1, contract2, contract3,
		contract1, contract2, contract3};

	// As no (or just a little) money is required to establish these
	// contracts, one can borrow some money from the bank and take on
	// contracts everywhere... What could possibly go wrong?
	//
	// This is really good work! The society really benefits by doing this.
	// Therefore, we reward traders with vast amounts of money...

	viac.value *= 0.5; // 50 % loss
	// Uh oh...

	double total_profit = 0;
	for(auto const& contract : contracts) {
		std::cout << contract;
		total_profit += contract.profitToInvestor();
	}

	std::cout << "Total profit: " << std::put_money(total_profit * 100.0) << std::endl;
	// Err, as bill got bankrupt, the losses are for the banks. And if
	// banks go bankrupt, the losses are for the society. Great, if you
	// earn money, you get rich, if you lose money, others will pay for it.
	// Sounds fair? Well, just a funny story of 2008, right? Won't happen
	// again.



	// Concluded, using {} for construction (almost) always works. I would
	// say, it is the preferred way.
	//
	// There is one construct that looks similar, which is an
	// std::initializer_list.  Sometimes you may get tricked between an
	// initializer list and an brace initializer.  But that one deserves
	// its own tip.
}

/*
 * Further reading:
 *
 * https://en.cppreference.com/w/cpp/language/initialization
 */

