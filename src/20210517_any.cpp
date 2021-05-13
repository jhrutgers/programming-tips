/*
 * any
 *
 *
 */

#include <any>

#include <functional>
#include <iostream>
#include <vector>

static void print(std::any const& song)
{
	if(auto const* si = std::any_cast<int>(&song))
		std::cout << *si << " (int)" << std::endl;
	else if(auto const* sc = std::any_cast<char const*>(&song))
		std::cout << *sc << " (char const*)" << std::endl;
	else if(auto const* ss = std::any_cast<std::string>(&song))
		std::cout << *ss << " (std::string)" << std::endl;
	else
		std::cout << "(unknown type " << song.type().name() << ")" << std::endl;
}

int main()
{
	std::any ahoy = 65;
	std::cout << std::any_cast<int>(ahoy) << std::endl;

	ahoy = "esc2020";
	std::cout << std::any_cast<char const*>(ahoy) << std::endl;

	ahoy.reset();

	ahoy = std::string("shine a light");
	std::cout << std::any_cast<std::string>(ahoy) << std::endl;


	auto lordi = [](){ return "Hard Rock Hallelujah"; };
	std::any esc2006 = lordi;
	try {
		std::cout << "Lordi: " << std::any_cast<std::function<char const*()>>(esc2006)() << std::endl;
	} catch(std::bad_any_cast const& e) {
		// bad cast
		std::cout << e.what();
	}

	esc2006 = std::make_any<std::function<char const*()>>(lordi);
	std::cout << "; " << std::any_cast<std::function<char const*()>>(esc2006)() << std::endl;

	// Strange... Valid in C++17, but not in 14.
	if(std::any wurst{Rise_like_a_Phoenix}; wurst.has_value()) {
		// Don't know what this is.
		print(wurst);
	}

	char Toy[] = "Toy";
	std::string Duncan{"Arcade"};

	std::vector<std::any> songs = { lordi, 1944, "Amar pelos dois", Toy, Duncan };

	for(auto const& song : songs)
		print(song);
}
