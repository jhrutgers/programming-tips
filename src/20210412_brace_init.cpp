/*
 * Brace initialization
 *
 * C++11 introduces a uniform way to initialize values and aggregates, and make
 * this more robust, compared to other ways of initialization.
 *
 * This program shows what you can do with it.
 */

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <string>

struct Bank {
	std::string name;
};

struct Investor {
	Investor() = default;

	Investor(std::string const& name)
		: m_name{name}
	{}

	Investor(std::string&& name)
		: m_name{std::move(name)}
	{}

	std::string const& name() const { return m_name; }

private:
	std::string const m_name{"anonymous"};
};

struct Company {
	// Direct initializer
	std::string name{10, '?'};
	// Value initializer (default construct)
	double value{};
};

struct Share {
	Company const& company;
	Bank const& owner;
	double amount{};

	double value() const { return company.value * amount; }
};

class Contract {
public:
	Contract(Bank const& bank, Investor const& investor, Company const& company, double amount, double costs)
		: m_bank{bank}
		, m_investor{investor}
		, m_share{company, bank, amount}
		, m_costs{costs}
		, m_initialValue{m_share.value()}
	{
	}

	double totalReturn() const
	{
		return m_share.value() - m_initialValue;
	}

	double profitToInvestor() const
	{
		return totalReturn() - profitToBank();
	}

	double profitToBank() const
	{
		return m_costs * m_initialValue;
	}

	friend std::ostream& operator<<(std::ostream& stream, Contract const& contract)
	{
		std::cout << std::showbase;
		std::cout.imbue(std::locale("en_US.UTF-8"));

		auto result = contract.m_share.value() / contract.m_initialValue - 1.0;
		stream << "Share of " << contract.m_share.company.name
			<< " changed value by " << result * 100.0 << "% to " << std::put_money(contract.m_share.value() * 100.0) << "; "
			<< contract.m_bank.name << " earns " << std::put_money(contract.profitToBank() * 100.0) << ", "
			<< contract.m_investor.name();

		auto profit = contract.profitToInvestor();
		if(profit >= 0)
			stream << " earns " << std::put_money(profit * 100.0);
		else
			stream << " loses " << std::put_money(-profit * 100.0);

		stream << std::endl;
		return stream;
	}

private:
	Bank const& m_bank;
	Investor const& m_investor;
	Share m_share;
	double m_costs{};
	double m_initialValue{};
};

int main()
{
	// Need to know the structure of the data type.
	Bank b = {"bank"};
	// Only valid with constructor
	Investor i("person");
	// Most vexing parse
	Investor anon();

	// Aggregate initialization
	Bank b2{"bank2"};
	Company c{"c", 10e9};
	// Direct initialization; constructor arguments
	Share s{c,b2};
	Investor anon2{};
	(void)s;
	(void)anon2;

	// Direct initialization
	Contract trs{b,i,c,0.01,0.02};
	// Copy initialization
	Contract trs2{trs};
	// List initialization
	Contract trss[]{trs,{b,i,c,0.01,30}};
	c.value = 5e9;
	std::cout << trss[0];
}

