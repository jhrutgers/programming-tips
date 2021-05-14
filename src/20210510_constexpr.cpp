/*
 * constexpr
 *
 * To make C++ fast, the compiler tries to optimize as much code as possible.
 * This would reduce the required work to do at run-time. Think of computing
 * expressions that are constant, initializing global/static variables, and
 * inlining small functions. Moreover, template evaluation is always done at
 * compile-time.
 *
 * You can help the compiler writing code that is suitable for compile-time
 * evaluation, and therefore saving time in run-time. For this, constexpr is
 * helpful.
 *
 * Read this file top-down. Execution is preferred. You can provide your own
 * inputs for this program.  In a normal cmake configuration, you can do this
 * like (details regarding the inputs will described below):
 *
 * mkdir build
 * cd build
 * cmake .. "-DTTM_INPUT=some_input"
 * cmake --build .
 * 20210510_constexpr "another_input"   <-- Make sure to have the correct path.
 */

// constexpr is a keyword. These includes are just for this example.
#include <array>
#include <exception>
#include <iostream>

// Templates are always evaluated at compile-time (they just don't exist
// anymore at run-time), so what is this fuzz about a new keyword? Well,
// templates can compute everything, but may not be very intuitive... The
// easiest way to prove this statement is to show how to implement a Turing
// machine (see https://en.wikipedia.org/wiki/Turing_machine) by means of
// templates.
//
// Yes! I was just looking for an excuse to write one :) So, here is my:
//
// * * * * * * * * * * * * * *
// * Template Turing Machine *
// * * * * * * * * * * * * * *
//
// (If you really don't care about templates and just want to know something
// about constexpr, you can skip this part.)
//
// --------------[ vv Cut here if you have limited run-time vv ]--------------

// Let's define the state of our Turing machine as an (enum of) int.  Upon
// startup, the state is always Init. When the state Halt is entered, execution
// stops.
enum State : int {
	Halt = -1,
	Init = 0,
};

// The machine has a large type, which holds the input (initially) and is
// converted to the output. A symbol on the tape is a char. It can be any char,
// but let's define a few specials.
enum Symbol : char {
	blank = ' ',	// Default symbol on an empty tape.
	zero = '0',
	one = '1',
	two = '2',
	any = '?',	// For input symbol matching, which matches anything.
	nothing = '*',	// For output symbol writing, which means 'don't write'.
	erased = '_',
};

// The head over the tape can be moved.
enum Move {
	Left,
	None,
	Right,
};



// This is an instruction for the machine. When the current state and the
// symbol on the tape under the head match (state and input template
// parameter), it will write the output symbol, move the tape, and proceed to
// the next state.
template <State state, Symbol input, Symbol output, Move move, State next>
struct Instruction {};

// A program is a list of instructions.
template <typename... Instruction>
struct Program {};



// The tape holds a list of symbols.
template <Symbol... symbols>
struct Tape {};

// This is a helper type to convert numbers into Symbols. It makes entering the
// input a bit easier.
template <int... values>
struct CastTape {
	using tape = Tape<static_cast<Symbol>(values)...>;
};



// Now a few 'functions' are defined to work with Tape<...> types.  Well, not
// really functions, but they can be used within template magic to accomplish
// something.



// A function to return the first element of the tape. The default definition
// is never used, the specializations are particularly interesting. Every
// specialization has a value member.  In case you make a programming error,
// the compiler will probably bail out on the fact that 'value' is missing when
// this definition is used.
template <typename Tape>
struct head {};

// In case of a non-empty tape, return the first symbol.
template <Symbol symbol, Symbol... symbols>
struct head<Tape<symbol, symbols...>> {
	static Symbol const value = symbol;
};

// In case of an empty tape, return a blank. This implicitly makes the tape
// longer. So, the tape has effectively an infinite length.
template<>
struct head<Tape<>> {
	static Symbol const value = blank;
};



// A function to drop the first element of the tape.
template <typename Tape>
struct dropFirst {};

// Specialization for a non-empty tape.
template <Symbol symbol, Symbol... symbols>
struct dropFirst<Tape<symbol, symbols...>> {
	using tape = Tape<symbols...>;
};

// Specialization for an empty tape.
template <>
struct dropFirst<Tape<>> {
	using tape = Tape<>;
};



// A function to append a symbol to the tape.
template <Symbol symbol, typename Tape>
struct append {};

// The specialization that is actually always used.
template <Symbol symbol, Symbol... symbols>
struct append<symbol, Tape<symbols...>> {
	using tape = Tape<symbols..., symbol>;
};



// A function to prepend a symbol to the tape.
template <Symbol symbol, typename Tape>
struct prepend {};

template <Symbol symbol, Symbol... symbols>
struct prepend<symbol, Tape<symbols...>> {
	using tape = Tape<symbol, symbols...>;
};



// A function to drop the last element of the tape.
template <typename Tape>
struct dropLast {};

// Specialization to scan till the last one is found.
template <Symbol symbol0, Symbol symbol1, Symbol... symbols>
struct dropLast<Tape<symbol0, symbol1, symbols...>> {
	// Great, this is a recursive type.  Recursion is fine, but the
	// compiler may limit the amount of it.  So, although the tape is in
	// this definition of infinite length, your compiler may supported only
	// some finite length.
	using tape = typename prepend<symbol0, typename dropLast<Tape<symbol1, symbols...>>::tape>::tape;
};

// Specialization to actually drop a symbol.
template <Symbol symbol>
struct dropLast<Tape<symbol>> {
	using tape = Tape<>;
};

// In case the tape was already empty, nothing has to be dropped.
template <>
struct dropLast<Tape<>> {
	using tape = Tape<>;
};



// A function to return the last element of the tape.
template <typename Tape>
struct last {};

// Specialization to skip to the last element.
template <Symbol symbol0, Symbol symbol1, Symbol... symbols>
struct last<Tape<symbol0, symbol1, symbols...>> {
	// Recursion!
	static Symbol const value = last<Tape<symbol1, symbols...>>::value;
};

// Specialization to return the last element.
template <Symbol symbol>
struct last<Tape<symbol>> {
	static Symbol const value = symbol;
};

// If the tape is empty, add a blank at the end. This effectively makes the
// tape of infinite length.
template <>
struct last<Tape<>> {
	static Symbol const value = blank;
};



// A function to replace the first symbol of the tape.
template <Symbol symbol, typename Tape>
struct set {
	using tape = typename prepend<symbol, typename dropFirst<Tape>::tape>::tape;
};

// A specialization in case 'nothing' is to be set.
template <typename Tape>
struct set<nothing, Tape> {
	using tape = Tape;
};



// Forward-declare the Machine type.  It has a program, the part of the tape
// left of the head, the part of the tape with the head and right of it, and
// the current state.
template <typename Program, typename TapeLeft, typename TapeRight, State state = State::Init>
struct Machine;



// The function to execute an instruction on the machine.
template <typename Machine, typename Instruction>
struct execute {
};

// Specialization for an instruction without moving.
template <
	typename Program, typename TapeLeft, typename TapeRight, State state,
	Symbol input, Symbol output, State next>
struct execute<Machine<Program, TapeLeft, TapeRight, state>, Instruction<state, input, output, Move::None, next>> {
	using result = Machine<Program, TapeLeft, typename set<output, TapeRight>::tape, next>;
};

// Specialization for an instruction with a move-left.
template <
	typename Program, typename TapeLeft, typename TapeRight, State state,
	Symbol input, Symbol output, State next>
struct execute<Machine<Program, TapeLeft, TapeRight, state>, Instruction<state, input, output, Move::Left, next>> {
	using result = Machine<Program,
		typename dropLast<TapeLeft>::tape,
		typename prepend<last<TapeLeft>::value, typename set<output, TapeRight>::tape>::tape,
		next>;
};

// Specialization for an instruction with a move-right.
template <
	typename Program, typename TapeLeft, typename TapeRight, State state,
	Symbol input, Symbol output, State next>
struct execute<Machine<Program, TapeLeft, TapeRight, state>, Instruction<state, input, output, Move::Right, next>> {
	using result = Machine<Program,
		typename append<head<typename set<output, TapeRight>::tape>::value, TapeLeft>::tape,
		typename dropFirst<TapeRight>::tape,
		next>;
};



// Find the instruction that matches the given state and input.  This default
// definition is not used. See the specializations below.
template <typename Program, State state, Symbol input>
struct find {};

// Specialization that matches the given state and input.
template <State state, Symbol input, Symbol output, Move move, State next, typename... MoreInstructions>
struct find<Program<Instruction<state, input, output, move, next>, MoreInstructions...>, state, input> {
	using instruction = Instruction<state, input, output, move, next>;
};

// Specialization that matches the given state and any input (actual input is
// ignored, when the instruction indicates to do so).
template <State state, Symbol input, Symbol output, Move move, State next, typename... MoreInstructions>
struct find<Program<Instruction<state, any, output, move, next>, MoreInstructions...>, state, input> {
	using instruction = Instruction<state, any, output, move, next>;
};

// If the first instruction of the program does not match, look for another one.
template <typename Instruction, State state, Symbol input, typename... MoreInstructions>
struct find<Program<Instruction, MoreInstructions...>, state, input> {
	// Another recursive type!
	using instruction = typename find<Program<MoreInstructions...>, state, input>::instruction;
};



// Here is the actual implementation of the machine.  The machine finds the
// next instruction that matches the current state and input (first element
// (the head) of the right part of the tape), and execute it...
template <typename Program, typename TapeLeft, typename TapeRight, State state>
struct Machine {
	// Recursion!
	using run = typename execute<Machine, typename find<Program, state, head<TapeRight>::value>::instruction>::result::run;
};

// ...until the Halt state is encountered.
template <typename Program, typename TapeLeft, typename TapeRight>
struct Machine<Program, TapeLeft, TapeRight, State::Halt> {
	using run = Machine;
};



// Finally, a few functions to show the tape, to actually give the output of
// the program. The show() functions have a parameter (by value). However,
// these structs are all empty, and are not used by the function. They are only
// used for overload and template parameter deduction.
static void show(Tape<> /*unused*/) {
	// Tape is empty. Nothing to show.
}

template <Symbol symbol, Symbol... symbols>
static void show(Tape<symbol, symbols...> /*unused*/) {
	// Note that the template parameter is printed here, not the Tape
	// instance that was passed as function parameter.
	std::cout << static_cast<char>(symbol);
	show(Tape<symbols...>());
}

template <typename Program, typename TapeLeft, typename TapeRight, State state>
static void show(Machine<Program, TapeLeft, TapeRight, state> /*unused*/) {
	show(TapeLeft());
	std::cout << " >"; // Head indicator.
	show(TapeRight());
	std::cout << std::endl;
}



// --------------[ ^^ Cut here if you have limited run-time ^^ ]--------------
//
// Ok, sorry, I got carried away by this Turing machine...
//
// Anyway, we now have a Machine type that expects a program, and a tape (split
// in a left and right part).  The tape holds a sequence of chars.  The input
// is always at the right part of the tape.

// This is our Template Turing Machine. It only needs a program and the input.
template <typename Program, typename Input>
using TTM = Machine<Program, Tape<>, Input>;

// Let's program the TTM such that it does a 'Normalized Measurement' of the
// size of the string at the input. The result is: <binary value>nm at the
// start of the tape. The input is erased (replaced by _). Only the input up
// the first space (blank) is processed.
//
// What Normalized Measurement exactly means, is to be defined.  It seems that
// not the full length is measured, but only some arbitrary part of it.
// Somehow, the world uses these Normalized Measurements often, but a good
// definition lacks. Anyway, let's see what is going to happen.
//
// Let's define a programmed TTM, with the Normalized Measurement program,
// which only needs the input.
template <typename Input>
using TTM_measure = TTM<
	Program<
		// Whatever is the input, first go write the '0nm'
		Instruction<State::Init, any, nothing, Move::Left, State(1)>,
		Instruction<State(1), blank, Symbol('m'), Move::Left, State(2)>,
		Instruction<State(2), blank, Symbol('n'), Move::Left, State(3)>,
		Instruction<State(3), blank, zero, Move::Right, State(4)>,
		// Move right to 'm'
		Instruction<State(4), Symbol('m'), nothing, Move::Right, State(5)>, // Got it.
		Instruction<State(4), any, nothing, Move::Right, State(4)>, // Still looking...
		// Move right to first non-erased symbol, erase and skip all other symbols than '1'.
		Instruction<State(5), one, erased, Move::Left, State(6)>,
		Instruction<State(5), blank, nothing, Move::None, State::Halt>, // Is this a problem?
		Instruction<State(5), any, erased, Move::Right, State(5)>, // Still looking...
		// Move right to 'n'
		Instruction<State(6), Symbol('n'), nothing, Move::Left, State(7)>, // Got it.
		Instruction<State(6), any, nothing, Move::Left, State(6)>, // Still looking...
		// Binary add one.
		Instruction<State(7), one, zero, Move::Left, State(7)>, // Continue with carry.
		Instruction<State(7), any, one, Move::Right, State(4)> // Done, continue with next input.
	>,
	Input
>;

// Ready for some tests?
void test_ttm()
{
	// Test run:
	// Measure the length of nothing.
	// Expected output: 0nm
	//
	// This statement defines the type 'TTM_measure', which gets one
	// template parameter 'Tape<>'.  This is a struct, which holds a type
	// 'run'. This 'run' type is a very complex (recursive) type, and
	// finally gives a Machine type. That resulting type is instantiated
	// (the final (), although {} would be better as a brace-init, but the
	// () more looks like a function call). Although this instance has no
	// members, so there is no data to be passed to the show() function,
	// show() will get the types and uses them to print the output.
	show(TTM_measure<Tape<>>::run());

	// The full TTM evaluation is done by the compiler. The show() function
	// is the only thing that is left for run-time. If you use objdump, for
	// example, you can verify this.

	// Now, test a few actual inputs.

	// Measure the length of 01101110001110101110011
	// Expected output: 1110nm (decimal: 14nm)
	show(TTM_measure<Tape<zero, one, one, zero, one, one, one, zero, zero, zero, one, one, one, zero, one, zero, one, one, one, zero, zero, one, one>>::run());

	// Wow! We have a working processor that operates really fast (well, it
	// only works at compile-time).  So, it has a perfect run-time
	// performance. We should think of a great marketing term for it.

	// Measure the length of 101012101232101210101
	// Expected output: 1010nm (decimal: 10nm)
	show(TTM_measure<CastTape<'1','0','1','0','1','2','1','0','1','2','3','2','1','0','1','2','1','0','1','0','1'>::tape>::run());

	// Measure the length of Th1s_1s_n1ce!!1111
	// Expected output: 111nm (decimal: 7nm)
	show(TTM_measure<CastTape<'T','h','1','s','_','1','s','_','n','1','c','e','!','!','1','1','1','1'>::tape>::run());

	// Now, your try:
	// Pass "-DTTM_INPUT=0x31,'1',49,061,'\x31'" (or any other sequence to
	// be Normalized Measured) to cmake and run the TTM.
#ifdef TTM_INPUT
	std::cout << "Your TTM run: ";
	show(TTM_measure<CastTape<TTM_INPUT>::tape>::run());
#endif

	// As the TTM runs at compile-time, you cannot pass data from the
	// command line (argv) to the TTM.
}



// Ok, this was not really what you were waiting for. Templates are great, but
// not trivial to write/read/understand for such a complex logic. As Turing
// machines are fun, we are doing it again, but now as:
//
// * * * * * * * * * * * * * * *
// * constexpr Turing Machine  *
// * * * * * * * * * * * * * * *
//
// The goal is the same as the TTM (do as much as possible on compile-time),
// but with different syntax.
//
// Marking variables and functions as constexpr indicates that the compiler can
// use them everywhere where a constant expression is required, such as in
// template parameters or within typedefs, or static_assert. Let's see how it
// works.

// This is a normal class.
class CTM {
public:
	enum {
		TapeLength = 128,
	};

	// This function is marked as constexpr. So, it can be executed by the
	// compiler.
	constexpr static size_t strlen(char const* s)
	{
		// In C++11, functions were only allowed to have a return
		// statement (no other statements).  That is not really helpful
		// for complex functions.
		//
		// In C++14, these requirements are relaxed. Basically, you can
		// do most logic, as long as you don't use resources like the
		// heap or input/output. So, you can't use std::string/
		// std::set/etc., as they use the heap internally.  All
		// functions you call from a constexpr function must also be
		// constexpr. Moreover, a constexpr function cannot be virtual.
		// And exceptions are not supported.
		//
		// From C++14 and later, most STL types are migrating to
		// supporting constexpr. Check the manual for details.

		// As std::strlen() is not constexpr, so do it ourselves.
		size_t len = 0;
		for(;*s;s++,len++);
		return len;
	}

	// This constructor is marked constexpr. By doing this, an instance of
	// this class can be computed at compile-time. In this case, the
	// compiler will execute this constructor. As a result, during run-time
	// the constructor won't be invoked, but the instance 'just exists'
	// already in memory.
	constexpr explicit CTM(char const* program)
		: m_program(program)
	{
		if(!program) {
			// Hey, exception were not supported?!? Indeed, as long
			// as they are not on the path of execution. So, when
			// this branch is not hit, the constructor compiles
			// fine and is usable as constexpr.  When during
			// constexpr evaluation this throw statement would be
			// hit, your compiler will complain that it is not
			// allowed to do so.  So, you can have error checking
			// by the compiler at compile-time.
			throw std::runtime_error("No program specified");
		}

		// The CTM program is a list of 5-tuples, which are just five
		// characters in a row.  Make sure the length of the program is
		// a multiple of five.
		if(strlen(program) % 5U != 0)
			throw std::runtime_error("Invalid program");
	}

	// This function is also constexpr. It can be called on an constexpr
	// instance of this class, but also on a normal instance. You will see
	// that later on.
	//
	// This function implements the Turing machine.
	constexpr auto run(char const* input) const
	{
		// std::array can be used in constexpr context.
		// In contrast to the TTM, the tape has finite length.
		std::array<char,TapeLength> tape{};

		// Initialize to blank.
		for(auto& c : tape)
			c = blank;

		// Copy input to tape.
		size_t head = tape.size() / 2;
		for(size_t i = 0; input[i] && i + head < tape.size(); i++)
			tape[head + i] = input[i];

		// Run program.
		char state = '0';
		while(true) {
			// Find instruction to run.
			char const* instr = m_program;
			for(; *instr; instr += 5U) {
				// Check state of instruction.
				if(instr[0] != state)
					continue;
				// Check input of instruction.
				if(instr[1] == any)
					break;
				if(instr[1] == tape[head])
					break;
			}

			if(!*instr)
				throw std::runtime_error("Invalid program");

			// Update tape.
			if(instr[2] != nothing)
				tape[head] = instr[2];

			// Move head.
			if(instr[3] == 'R') { // Move head to the right.
				if(head >= tape.size() - 1)
					throw std::runtime_error("Out of memory");

				head++;
			} else if(instr[3] == 'L') { // Move head to the left.
				if(head == 0)
					throw std::runtime_error("Out of memory");

				head--;
			}

			// Update state.
			state = instr[4];
			if(state == 'H') // Halt
				return tape;
		}
	}

private:
	char const* m_program;
};

static void show(std::array<char,CTM::TapeLength> const& tape)
{
	// Tape is not null-terminated.
	std::string s{tape.data(), tape.size()};

	// Erase blanks at the beginning and end of the tape.
	s.erase(0, s.find_first_not_of(" "));
	s.erase(s.find_last_not_of(" ") + 1);

	std::cout << s.c_str() << std::endl;
}

// Need a reference to a variable as template argument (can't use std::array as value).
template <std::array<char,CTM::TapeLength> const& tape>
static void show()
{
	show(tape);
}

// Here we go: a constexpr instance of CTM, provided with a program.  Define
// the Normal Measurement program, just like TTM_measure.
//
// Construction of the CTM instance is done by the compiler. If you make a typo
// in the program, you can see how your compiler handles that.
constexpr static CTM ctm_measure{
	"0?*L1"
	"1 mL2"
	"2 nL3"
	"3 0R4"
	"4m*R5"
	"4?*R4"
	"51_L6"
	"5 *NH" // Still not a problem.
	"5?_R5"
	"6n*L7"
	"6?*L6"
	"710L7"
	"7?1R4"
// Enable this line to create an invalid program.
#if 0
	"huh?"
#endif
};

// Another constexpr variable, which actually runs the machine with some input.
// The output (an std::array<> instance) is computed by the compiler.
//
// Expected output: 11nm (decimal: 3nm)
constexpr static auto tape = ctm_measure.run("What_1s_the_w1dth_of_6_Si_atoms?!1");

static void test_ctm(char const* input)
{
	// Here, pass a reference to the constexpr tape to the show() function.
	// In this case, we pass it as a template parameter, to make sure that
	// the compiler has to compute it at compile time.
	//
	// tape must be known by the linker, so it must exist as global instead
	// of a local variable. Verify using objdump that indeed the output of
	// ctm_measure.run() is already in your binary and not computed at
	// run-time.
	show<tape>();

	// Great! Another compile-time processor!

	// Now your turn: pass 'IBM_d1d_it!1' as command line argument.
	if(input) {
		std::cout << "Your CTM run: ";
		// This will be executed as normal run-time computation.  That
		// is great, as it uses exactly the same code as for the
		// constexpr variables.
		show(ctm_measure.run(input));
	}
}


// main
int main(int argc, char** argv)
{
	test_ttm();
	test_ctm(argc >= 2 ? argv[1] : nullptr);
}

// These examples may be a bit complex, but show that you can do anything at
// compile-time.
//
// Templates are great, but using constexpr for this may be better, because:
// - the syntax is more what you would expect in a C++-program;
// - the same code can be used for run-time execution;
// - it is (probably) easier to write and debug.

/*
 * Further reading:
 *
 * What you can and cannot do from constexpr functions:
 * https://en.cppreference.com/w/cpp/language/constexpr
 *
 * More on constant expressions:
 * https://en.cppreference.com/w/cpp/language/constant_expression
 */


