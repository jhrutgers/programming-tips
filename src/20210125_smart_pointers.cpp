/*
 * Smart pointers
 *
 * Memory management can be tricky in C++. Smart pointers may help you with it,
 * as they keep track of whether an object is still in use. If the last user
 * discards the pointer to the object, the object is deleted automatically.
 *
 * This example shows some basic use of a few of these smart pointers.
 *
 * Scroll down to main() and read in the program in execution order.
 */

// This is the header you need for std::shared_ptr and friends.
#include <memory>

#include <string>
#include <vector>
#include <deque>
#include <cassert>

class Person {
public:
	Person(char const* name) : name(name) {}
	std::string name;
};

class Administration {
public:
	Administration(std::initializer_list<std::shared_ptr<Person>> l)
		: people(l.begin(), l.end())
	{}

	// Check this out, you can also put std::shared_ptr in a collection.
	// You can also put std::unique_ptr in here.
	std::deque<std::shared_ptr<Person>> people;

	std::shared_ptr<Person> const& president() {
		return *people.begin();
	}
};

class Campaign {
public:
	Campaign(std::shared_ptr<Person> const& p, std::shared_ptr<Person> const& vp)
		: president(p), vice_president(vp)
	{}

	std::shared_ptr<Person> president;
	std::shared_ptr<Person> vice_president;

	operator std::unique_ptr<Administration>() {
		return std::unique_ptr<Administration>(
			new Administration({president, vice_president}));
	}
};

struct Dirt {};


// A global variable, default initialized to nullptr.
std::unique_ptr<Administration> us_cabinet;

auto election_2016(std::unique_ptr<Campaign> const& stronger_together, std::unique_ptr<Campaign> make_america_great_again)
{
	try {
		throw std::vector<Dirt>({Dirt(), Dirt(), Dirt()});
	} catch(...) {
		// Dereferencing a std::shared_ptr and std::unique_ptr returns
		// a reference to the object that was pointed to.  In this
		// case, it returns a Campaign&. The Campaign class implements
		// a conversion to an std::unique<Cabinet>, which then is used
		// in the assignment to us_cabinet.
		us_cabinet = *make_america_great_again;
	}

	// This will become a std::unique<Dirt>.
	auto speech = std::make_unique<Dirt>();

	// This makes the return type of this function a Person*, which is an
	// unguarded/naked pointer. However, when speech is going out of scope,
	// the std::unique_ptr will delete the object...
	return speech.get();
}

void election_2020(std::unique_ptr<Campaign>&& keep_america_great_again, std::unique_ptr<Campaign> build_back_better)
{
	try {
		throw Dirt();
	} catch(...) {
		// ignore
	}

	us_cabinet = *build_back_better;
}

template <typename T>
static void believe(T* x)
{
	// This is dangerous, as the pointer is not guarded by std::shared_ptr
	// and friends.  So, the behavior is undefined when x is accessed.
}

int main()
{
	// An std::shared_ptr instances holds a pointer to an object. It can be
	// duplicated, such that multiple instances of std::shared_ptr can
	// point to the same object. If all these std::shared_ptr instances are
	// destroyed (tracked by reference counting), the object they pointed
	// to is deleted automatically.
	//
	// std::unique_ptr assumes that there is only one pointer to the
	// object.  If a std::unique_ptr goes out of scope, it deletes the
	// object it points to.  You cannot duplicate std::unique_ptr, as you
	// can with std::shared_ptr.  However, you can pass ownership between
	// std::unique_ptr instances.
	//
	// Both classes have a similar interface.  Use ptr.get() to get the
	// actual saved pointer, and ptr.reset() to release the previous
	// pointer and save another one.
	//
	// If you do it well, you won't have to use new and delete anymore in
	// your code. Let's check the example.

	// These lines are all equivalent:
	// - construct by pointer
	std::shared_ptr<Person> kaine(new Person("Kaine"));
	std::shared_ptr<Person> pence(new Person{"Pence"});
	// - construct by copy
	std::shared_ptr<Person> clinton = std::shared_ptr<Person>(new Person("Clinton"));
	// - use std::make_shared, which will do the new for you. The arguments
	//   are passed to the Person's constructor.
	std::shared_ptr<Person> biden = std::make_shared<Person>("Biden");
	// - auto-deduct type (preferred syntax)
	auto harris = std::make_shared<Person>("Harris");

	// This one is a bit unique. Note the std::unique_ptr to
	// std::shared_ptr conversion.  std::make_unique returns an
	// std::unique_ptr. When it is assigned to the std::shared_ptr, the
	// std::unique_ptr releases the ownership.
	std::shared_ptr<Person> trump = std::make_unique<Person>("Trump");

	// In all examples above, the six instances of std::shared_ptr hold the
	// only reference to the six instances of Person. If they get out of
	// scope (at the end of main()), the Person instances are cleaned up
	// automatically. So, no delete and no memory leaks anywhere!


	// You can pass the smart pointers as function arguments.  In this
	// case, std::unique_ptrs are constructed, which holds internally a
	// std::shared_ptr to the same Persons.
	auto speech = election_2016(
		std::make_unique<Campaign>(clinton, kaine),
		std::make_unique<Campaign>(trump, pence));

	std::shared_ptr<Dirt> fact;
	for(int i = 0; i < 1461; i++) {
		// This new is kind of fake, as it is automatically deleted in the next iteration.
		fact.reset(new Dirt());
	}

	// Note how speech's type is resolved as a pointer, but the object was already deleted.
	believe(speech);

	election_2020(
		std::make_unique<Campaign>(trump, pence),
		std::make_unique<Campaign>(biden, harris));

	// If the std::shared_ptr is reset here, there is no problem at all, as
	// the Person instance is also used by other instances of
	// std::shared_ptr and just stays alive.
	biden.reset();

	// A smart pointer behaves a bit like a normal pointer. You can use ->
	// on the instance, and it implements a conversion to bool, so you can
	// use it in an expression, without explicitly calling get() to get the
	// stored pointer.
	assert(us_cabinet->president());

	return 47;
}

/*
 * Further reading:
 *
 * https://en.cppreference.com/w/cpp/memory/shared_ptr
 * https://en.cppreference.com/w/cpp/memory/unique_ptr
 *
 * Break dependency cycles using weak_ptr:
 * https://en.cppreference.com/w/cpp/memory/weak_ptr
 */

