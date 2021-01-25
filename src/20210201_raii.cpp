/*
 * Resource Acquisition Is Initialization (RAII)
 *
 * Basically, you tie the acquisition and release of a resource to the lifetime
 * of an object. So, in the constructor of an object, you do the acquisition,
 * and in the destructor the release.
 *
 * A resource can be anything, like memory, a mutex, a file handle, etc.  By
 * using RAII, you cannot forget releasing the resource, so no memory leaks,
 * deadlocks, etc.
 *
 * Have a look how it works. Scroll down and start reading at main(). Follow
 * the execution order. There is no need to execute the program; nothing is
 * printed anyway.
 */

#include <cinttypes>
#include <stdexcept>
#include <cassert>

// Some resources, which are to be acquired/and released by Plan.
class Oss {
public:
	using Doses = int64_t;

	~Oss() {
		assert(m_reserved == 0);
	}

	void store(Doses x) noexcept {
		m_available += x;
	}

	void reserve(Doses x) {
		if(x > available())
			throw std::underflow_error("not available");
		m_reserved += x;
	}

	void giveup(Doses x) {
		if(x > m_reserved)
			throw std::underflow_error("not reserved");
		m_reserved -= x;
	}

	void take(Doses x) {
		if(x > m_available)
			throw std::underflow_error("not stored");
		m_available -= x;
	}

	Doses available() const { return m_available - m_reserved; }

private:
	Doses m_available = 0;
	Doses m_reserved = 0;
};

// There is only one place where resources are stored.
static Oss storage;

// This is the encapsulation of a resource (a number of Oss::Doses).  The
// constructor acquires the resource, and throws an exception when that is not
// possible.  Moreover, a resource should not get lost. This class makes sure
// that whatever happens, the resource is properly released.
//
// That sounds hard, but really isn't. The destructor is never* bypassed; it is
// properly invoked when the object is going out of scope, when an exception is
// handled, when instances are copied or moved in and out of STL containers,
// etc. So, basically implement a proper destructor.
//
// * OK, it is in one occasion: when the constructor throws an exception. Then,
//   the destructor is not run! But the object wasn't fully constructed in that
//   case.
class Plan {
public:
	using Doses = Oss::Doses;

	explicit Plan(Doses x = 0)
		: m_reserved(x)
	{
		storage.reserve(x);
	}

	// Resources cannot be duplicated. Explicitly remove the copy
	// constructor and assignment operator.
	Plan(Plan const&) = delete;
	void operator=(Plan const&) = delete;

	// Resources may be moved, though; take the given instance's resources,
	// make it ours and leave the given instance as an empty shell.
	Plan(Plan&& plan) {
		*this = std::move(plan);
	}

	Plan& operator=(Plan&& plan) {
		storage.giveup(m_reserved);
		m_reserved = plan.m_reserved;
		plan.m_reserved = 0;
		return *this;
	}

	// As the destructor may be called while unwinding the stack during
	// exception handling, make sure it does not throw any exceptions.
	// Therefore, mark it as noexcept.
	~Plan() noexcept {
		try {
			storage.giveup(m_reserved);
		} catch(std::overflow_error const&) {
			// It should not happen.
			assert(false);
		}
	}

	void exec() {
		storage.take(m_reserved);
		storage.giveup(m_reserved);
		m_reserved = 0;
	}

private:
	Doses m_reserved = 0;
};

Plan speak_up() {
	return Plan(314159);
}

int main()
{
	// Let's create some resources.
	storage.store(25000);

	// Try make Plans. While doing that, you may get exceptions.
	try {
		// Non-RAII example.

		Oss::Doses initial_plan = 10000;
		storage.reserve(initial_plan); // OK

		Oss::Doses stupid_plan = 100000;
		storage.reserve(stupid_plan); // ERROR

		// We won't get here. The initial_plan's resources are lost.
		// This is bad. This is what we have to prevent using RAII.

		storage.take(initial_plan);
		storage.giveup(initial_plan);

		storage.take(stupid_plan);
		storage.giveup(stupid_plan);
	} catch(std::underflow_error const&) {
		// Really tricky to recover from the error to prevent leakage...
		storage.giveup(10000);
	}

	// Try again, now with RAII.
	try {
		Plan some_really_vulnerable_people(10000); // OK
		Plan more_really_vulnerable_people(100000); // ERROR

		// We won't get here, but all resources are properly released
		// before executing the catch() part. No leakage. Neat, isn't?

		some_really_vulnerable_people.exec();
		more_really_vulnerable_people.exec();
	} catch(std::underflow_error const&) {
	}

	storage.store(375000);

	Plan important_medical_staff(30000 * 2);
	// At this point, the important_medical_staff holds a set of resources.
	// The implementation of exec() uses and releases the resources.
	// Afterwards, important_medical_staff becomes effectively an empty
	// shell without resources.  That's OK. Its destructor can handle that.
	//
	// How you handle resources, all depends on the application. That's not
	// part of RAII.
	important_medical_staff.exec();

	// The Plan is just a normal class. You can use all operations on the
	// object that you could do on any other C++ object. RAII does not make
	// it any different.
	try {
		Plan people_who_think_they_are_important(100000);
		Plan egoistic_people(1000000);
		Plan people_who_claim_to_be_essential(2500000);

		// Note that speak_up() returns a Plan. As the object is not
		// saved, it is std::move()d automatically into the Plan at the
		// left-hand side.
		Plan people_who_get_bold = speak_up();

		// Next line doesn't work, as a Plan cannot be copied.
		//Plan people_who_want_first = people_who_think_they_are_important;

		// But you can move it:
		Plan people_who_want_first = std::move(people_who_think_they_are_important);
	} catch(...) {
		// Ignore. As every Plan is safe in resource handling, we can
		// just forget about them and continue; all the resources they
		// claimed are just still made available for later.
	}



	// Well, this can continue for a while...

	storage.store(2500000);

	Plan medical_staff(273000 * 2);
	medical_staff.exec();

	Plan vulnerable_people(155000 * 2);
	vulnerable_people.exec();

	storage.store( 400000);
	Plan even_more_vulnerable_people(77000 * 2);

	storage.store(4500000 * 0.4);
	Plan more_medical_staff(258000 * 2);
	Plan even_more_medical_staff(204000 * 2);
	Plan less_important_medical_staff(25000 * 2);
	Plan more_vulnerable_people(60000 * 2);
}

/*
 * Further reading:
 *
 * https://en.cppreference.com/w/cpp/language/raii
 */

