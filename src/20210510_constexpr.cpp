/*
 * constexpr
 */

#include <array>
#include <exception>
#include <iostream>

// Template Turing Machine

enum Move {
	Left,
	L = Left,
	None,
	N = None,
	Right,
	R = Right,
};

enum State : int {
	Halt = -1,
	Init = 0,
};

enum Symbol : char {
	blank = ' ',
	zero = '0',
	one = '1',
	two = '2',
	any = '?',
	nothing = '*',
	erased = '_',
};

template <State state, Symbol input, Symbol output, Move move, State next>
struct Instruction {};

template <typename... Instruction>
struct Instructions {};

template <Symbol... symbols>
struct Tape {};

template <int... values>
struct CastTape {
	using tape = Tape<static_cast<Symbol>(values)...>;
};

template <typename Tape>
struct head {};

template <Symbol symbol, Symbol... symbols>
struct head<Tape<symbol, symbols...>> { static Symbol const value = symbol; };

template<>
struct head<Tape<>> { static Symbol const value = blank; };

template <typename Instructions, State state, Symbol input>
struct find {};

template <State state, Symbol input, Symbol output, Move move, State next, typename... MoreInstructions>
struct find<Instructions<Instruction<state,input,output,move,next>, MoreInstructions...>, state, input> {
	using instruction = Instruction<state, input, output, move, next>;
};

template <State state, Symbol input, Symbol output, Move move, State next, typename... MoreInstructions>
struct find<Instructions<Instruction<state,any,output,move,next>, MoreInstructions...>, state, input> {
	using instruction = Instruction<state, any, output, move, next>;
};

template <typename Instruction, State state, Symbol input, typename... MoreInstructions>
struct find<Instructions<Instruction,MoreInstructions...>, state, input> {
	using instruction = typename find<Instructions<MoreInstructions...>, state, input>::instruction;
};

template <typename Tape>
struct dropFirst {};

template <Symbol symbol, Symbol... symbols>
struct dropFirst<Tape<symbol, symbols...>> {
	using tape = Tape<symbols...>;
};

template <>
struct dropFirst<Tape<>> { using tape = Tape<>; };

template <Symbol symbol, typename Tape>
struct append {};

template <Symbol symbol, Symbol... symbols>
struct append<symbol, Tape<symbols...>> {
	using tape = Tape<symbols..., symbol>;
};

template <Symbol symbol, typename Tape>
struct prepend {};

template <Symbol symbol, Symbol... symbols>
struct prepend<symbol, Tape<symbols...>> {
	using tape = Tape<symbol, symbols...>;
};

template <typename Tape>
struct dropLast {};

template <Symbol symbol0, Symbol symbol1, Symbol... symbols>
struct dropLast<Tape<symbol0, symbol1, symbols...>> {
	using tape = typename prepend<symbol0, typename dropLast<Tape<symbol1, symbols...>>::tape>::tape;
};

template <Symbol symbol>
struct dropLast<Tape<symbol>> {
	using tape = Tape<>;
};

template <>
struct dropLast<Tape<>> {
	using tape = Tape<>;
};

template <typename Tape>
struct last {};

template <Symbol symbol0, Symbol symbol1, Symbol... symbols>
struct last<Tape<symbol0, symbol1, symbols...>> {
	static Symbol const value = last<Tape<symbol1, symbols...>>::value;
};

template <Symbol symbol>
struct last<Tape<symbol>> {
	static Symbol const value = symbol;
};

template <>
struct last<Tape<>> {
	static Symbol const value = blank;
};

template <Symbol symbol, typename Tape>
struct set {
	using tape = typename prepend<symbol, typename dropFirst<Tape>::tape>::tape;
};

template <typename Tape>
struct set<nothing, Tape> {
	using tape = Tape;
};

template <typename Instructions, typename TapeLeft, typename TapeRight, State state = State::Init>
struct Machine;

template <typename Machine, typename Instruction>
struct execute {
};

template <
	typename Instructions, typename TapeLeft, typename TapeRight, State state,
	Symbol input, Symbol output, State next>
struct execute<Machine<Instructions, TapeLeft, TapeRight, state>, Instruction<state, input, output, Move::None, next>> {
	using result = Machine<Instructions, TapeLeft, typename set<output, TapeRight>::tape, next>;
};

template <
	typename Instructions, typename TapeLeft, typename TapeRight, State state,
	Symbol input, Symbol output, State next>
struct execute<Machine<Instructions, TapeLeft, TapeRight, state>, Instruction<state, input, output, Move::Left, next>> {
	using result = Machine<Instructions,
		typename dropLast<TapeLeft>::tape,
		typename prepend<last<TapeLeft>::value, typename set<output, TapeRight>::tape>::tape,
		next>;
};

template <
	typename Instructions, typename TapeLeft, typename TapeRight, State state,
	Symbol input, Symbol output, State next>
struct execute<Machine<Instructions, TapeLeft, TapeRight, state>, Instruction<state, input, output, Move::Right, next>> {
	using result = Machine<Instructions,
		typename append<head<typename set<output, TapeRight>::tape>::value, TapeLeft>::tape,
		typename dropFirst<TapeRight>::tape,
		next>;
};

template <typename Instructions, typename TapeLeft, typename TapeRight, State state>
struct Machine {
	using run = typename execute<Machine, typename find<Instructions, state, head<TapeRight>::value>::instruction>::result::run;
};

template <typename Instructions, typename TapeLeft, typename TapeRight>
struct Machine<Instructions, TapeLeft, TapeRight, State::Halt> {
	using run = Machine;
};

static void show(Tape<>) {
}

template <Symbol symbol, Symbol... symbols>
static void show(Tape<symbol, symbols...>) {
	std::cout << static_cast<char>(symbol);
	show(Tape<symbols...>());
}

template <typename Instructions, typename TapeLeft, typename TapeRight, State state>
static void show(Machine<Instructions, TapeLeft, TapeRight, state>) {
	show(TapeLeft());
	std::cout << " >";
	show(TapeRight());
	std::cout << std::endl;
}

template <typename Instructions, typename Input>
using TTM = Machine<Instructions, Tape<>, Input>;

// Program the TTM such that it does a 'Normalized Measurement' of the size of
// the string at the input. The result is: <binary value>nm at the start of the
// tape. The input is erased (replaced by _). Only the input up the first space
// is processed. What Normalized Measurement exactly means, is to be defined.
// It seems that not the full length is measured, but only some arbitrary part
// of it.
template <typename Input>
using TTM_measure = TTM<
	Instructions<
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

void test_ttm()
{
	// Measure the length of nothing.
	// Expected output: 0nm
	show(TTM_measure<Tape<>>::run());

	// Measure the length of 01101110001110101110011
	// Expected output: 1110nm (decimal: 14nm)
	show(TTM_measure<Tape<zero, one, one, zero, one, one, one, zero, zero, zero, one, one, one, zero, one, zero, one, one, one, zero, zero, one, one>>::run());

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
}

// Constexpr Turing Machine

class CTM {
public:
	enum {
		TapeLength = 128,
	};

	constexpr static size_t strlen(char const* s)
	{
		// std::strlen() is not constexpr, so do it ourselves.
		size_t len = 0;
		for(;*s;s++,len++);
		return len;
	}

	constexpr explicit CTM(char const* program)
		: m_program(program)
	{
		if(!program)
			throw std::runtime_error("No program specified");

		// The program is a list of 5-tuples, which are just five
		// characters in a row.  Make sure the length of the program is
		// a multiple of five.
		if(strlen(program) % 5u != 0)
			throw std::runtime_error("Invalid program");
	}

	constexpr auto run(char const* input) const
	{
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
			for(; *instr; instr += 5u) {
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
			if(instr[3] == 'R') {
				if(head >= tape.size() - 1)
					throw std::runtime_error("Out of memory");

				head++;
			} else if(instr[3] == 'L') {
				if(head == 0)
					throw std::runtime_error("Out of memory");

				head--;
			}

			// Update state.
			state = instr[4];
			if(state == 'H')
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

	std::cout << s << std::endl;
}

// Need a reference to a variable as template argument (can't use std::array as value).
template <std::array<char,CTM::TapeLength> const& tape>
static void show()
{
	show(tape);
}

constexpr static CTM ctm_measure{
	// Identical program as used for the TTM_measure.
	"0?*L1"
	"1 mL2"
	"2 nL3"
	"3 0R4"
	"4m*R5"
	"4?*R4"
	"51_L6"
	"5 *NH"
	"5?_R5"
	"6n*L7"
	"6?*L6"
	"710L7"
	"7?1R4"
};

// Expected output: 11nm (decimal: 3nm)
constexpr static auto tape = ctm_measure.run("What_1s_the_w1dth_of_6_Si_atoms?!1");

static void test_ctm(char const* input)
{
	// tape must be known by the linker, so it must exist as global.
	show<tape>();
	// Great! Another compile-time processor!

	// Now your turn, pass 'IBM_d1d_it!1' as command line argument.
	if(input) {
		std::cout << "Your CTM run: ";
		// Just normal runtime computation.
		show(ctm_measure.run(input));
	}
}


// main
int main(int argc, char** argv)
{
	test_ttm();
	test_ctm(argc >= 2 ? argv[1] : nullptr);
}

