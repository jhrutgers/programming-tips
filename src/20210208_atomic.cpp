/*
 * std::atomic
 *
 * When using concurrency, communication using shared memory is way trickier
 * than you would expect. As threading is part of C++ since C++11, there are a
 * few things you should know.
 *
 * First of all, don't use threads. Use fibers instead (such as
 * https://github.com/jhrutgers/zth). However, there are a few cases where you
 * would still encounter shared memory in combination with concurrency, namely:
 *
 * - async signals (POSIX)
 * - interrupt handler (on a microcontroller)
 * - frameworks that still use threads (like Qt)
 * - people who never heard of fibers
 *
 * In all of these cases, the application can be interrupted at any point in
 * time, where another thread/context takes over and accesses the same (shared)
 * memory. Or worse, the thread runs on another core and access the same memory
 * at the same time.
 *
 * Now, you encounter two issues here:
 *
 * 1. If you want to communicate more than one byte at once, not all of them
 * may be received at the same time.  And it is annoying if you write a double
 * (8 bytes), and the other thread sees 4 bytes of the previous value and 4
 * bytes of the new value.
 *
 * 2. If you want to do arithmetic on the shared data, such as increment by
 * one.  You try to do a read-modify-write operation, but if the other thread
 * access the data while you were modifying, the result may be bogus. For
 * example, reference counting requires atomic increment and decrement.
 *
 * 3. Writes of one thread gets reordered when seen by another thread. OK, fair
 * enough, you won't encounter it, as you probably never see that happening.
 * The application may just crash.
 *
 * std::atomic will help you in these cases. Scroll down to main() and read in
 * execution order.  There is no need to run the program; nothing is printed
 * anyway.
 */

// std::atomic is defined here.
#include <atomic>

#include <vector>
#include <thread>
#include <cstdint>
#include <cassert>

class Hydrocarbon {
public:
	// To be used within std::atomic, make sure to have the default
	// copy/move constructor and copy/move assignment.  No special tricks
	// on memory layout; just use members that can be set without
	// side-effects.

	uintmax_t carbon = 0;
	uintmax_t hydrogen = 0;

	uintmax_t weight() const { return hydrogen + carbon * 12; }
};

int main()
{
	// Basically, wrap the data that needs to be thread-safe (or safe
	// between signals/interrupts) in an std::atomic. That works best for
	// integral values (int, unsigned long, etc.)
	std::atomic<long long> sun;
	// You can use it as you do daily with these types. The behavior you
	// observe is what you would expect. So, at the start, sun is not
	// initialized, as a long long never is by default.
	sun = 2;              // write
	sun++;                // read-modify-write
	sun += 1;             // read-modify-write
	++sun;                // read-modify-write
	sun.fetch_add(1);     // Equivalent to sun++;
	sun -= 4;             // read-modify-write
	long long rest = sun; // read
	(void)rest; // We don't need the rest for now.

	// What just happened? All these read and writes are done atomically;
	// whenever another thread would read sun, it either sees the old or
	// new value, but never parts of both.
	//
	// Moreover, read-modify-write is also performed atomically.
	//
	// That's basically it. If you are in a hurry, that's all you need to
	// know.




	// But if you look closer, the world is a bit more complex. std::atomic
	// has the operators overloaded as used above.  The equivalent function
	// names are there also.
	std::atomic<int> borssele;
	borssele.store(235);
	borssele.fetch_add(1); // "first fetch the value, then add and write back, and return the old value"
	borssele.fetch_sub(92);
	rest = borssele.load();
	(void)rest; // Discard.

	// Big warning: std::atomic allows you to do more complex stuff, namely
	// control memory ordering. But you should not use it! It is too
	// complex and requires a deep understanding of memory consistency
	// models before you can do it right.
	//
	// Memory ordering is about what the compiler and CPU is allowed to
	// reorder to before or after a certain operation on a std::atomic.
	// There are several flavors, which can speed up your application if
	// you do it right.  However, you probably won't know when you made a
	// mistake. Bugs in memory ordering only happen when a context switch
	// happens at exactly the right clock cycle, or when two reads of
	// different cores happen at (almost) exactly the same time. As that is
	// hardly ever the case, and certainly not reproducible, you won't be
	// able to find the bugs, even though the application crashes once in a
	// while.
	//
	// The default memory ordering is like "make sure it is in the order
	// the programmer specified", at the cost of a few memory barrier
	// instructions. This is fine, until you prove otherwise.
	std::atomic<int> iter;
	// So, don't poke around...
	iter.store(2, std::memory_order_relaxed);
	// ...in something that saves the world...
	iter.fetch_add(3, std::memory_order_acquire);
	// ...and promises great performance at low costs...
	iter.fetch_sub(4, std::memory_order_consume);
	// ...but you don't understand.
	rest = iter.load(std::memory_order_release);
	// We need it, but normal people can't grasp its greatness.
	(void)rest;



	// The examples above don't have threading, so you had to believe that
	// it worked.  We can run some tests and see what really happens.
	std::atomic<int_fast16_t> chernobyl(0);

	// You shouldn't use threads (use fibers instead), but for this example
	// we try dangerous stuff.
	std::vector<std::thread> test;
	// Kind of explosion of test threads.
	for(int i = 0; i < 86; i++)
		test.emplace_back([&]() {
			// This is executed in the thread. A read-modify-write.
			// If that would not be atomic, all threads could read
			// at the same time (read all 0), increment at the same
			// time (0 + 1), and write the result back (write all
			// 1).
			chernobyl++;
		});

	for(auto& t : test)
		t.join(); // How it comes all together...

	// Check if the ++ was indeed atomic for all threads...
	assert(chernobyl == 86);
	// Glad we survived the test. Now, don't use threads anymore.




	// std::atomic can be implemented in two ways for a certain type:
	// - using CPU instructions that make operations atomic;
	// - implement a mutex around the operations.
	//
	// The former is the fastest, safest, lock-free solution.  The latter
	// may lead to deadlock when waiting for a lock from a signal or
	// interrupt handler. It is not specified how std::atomic is
	// implemented in your setup, but you can ask.
	//
	// I strongly suggest to add something like this to your code to make
	// sure that only the instructions are used and not the mutex.
	assert(chernobyl.is_lock_free());

	// You could use other types with std::atomic, like this:
	Hydrocarbon methane{1,4};
	Hydrocarbon ethane{2,6};
	std::atomic<Hydrocarbon> stuff;
	stuff = methane; // atomic
	stuff = ethane;
	(void)stuff;

#if 0
	// These asserts will fail.
	assert(stuff.is_lock_free());
#if __cplusplus >= 201703L // C++17
	static_assert(std::atomic<Hydrocarbon>::is_always_lock_free);
#endif
#endif
}

