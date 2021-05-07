/*
 * constexpr
 */

#include <iostream>

// template Turing machine

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

using Symbol = int;

template <State state, Symbol input, Symbol output, Move move, State next>
struct Instruction {};

template <typename... Instruction>
struct Instructions {};

template <Symbol... symbols>
struct Tape {};

template <size_t N, Symbol... symbols>
struct EmptyTape {
	using tape = typename EmptyTape<N - 1, false, symbols...>::tape;
};

template <Symbol... symbols>
struct EmptyTape<0, symbols...> {
	using tape = Tape<symbols...>;
};

template <typename Tape>
struct head {};

template <Symbol symbol, Symbol... symbols>
struct head<Tape<symbol, symbols...>> { static Symbol const value = symbol; };

template <typename Instructions, State state, Symbol input>
struct find {};

template <State state, Symbol input, Symbol output, Move move, State next, typename... MoreInstructions>
struct find<Instructions<Instruction<state,input,output,move,next>, MoreInstructions...>, state, input> {
	using instruction = Instruction<state, input, output, move, next>;
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

template <Symbol symbol, typename Tape>
struct set {
	using tape = typename prepend<symbol, typename dropFirst<Tape>::tape>::tape;
};

template <typename Instructions, typename TapeLeft, typename TapeRight, State state = State::Init>
struct Machine;

template <typename Machine, typename Instruction>
struct apply {
};

template <
	typename Instructions, typename TapeLeft, typename TapeRight, State state,
	Symbol input, Symbol output, State next>
struct apply<Machine<Instructions, TapeLeft, TapeRight, state>, Instruction<state, input, output, Move::None, next>> {
	using result = Machine<Instructions, TapeLeft, typename set<output, TapeRight>::tape, next>;
};

template <
	typename Instructions, typename TapeLeft, typename TapeRight, State state,
	Symbol input, Symbol output, State next>
struct apply<Machine<Instructions, TapeLeft, TapeRight, state>, Instruction<state, input, output, Move::Left, next>> {
	using result = Machine<Instructions,
		typename dropLast<TapeLeft>::tape,
		typename prepend<last<TapeLeft>::value, typename set<output, TapeRight>::tape>::tape,
		next>;
};

template <
	typename Instructions, typename TapeLeft, typename TapeRight, State state,
	Symbol input, Symbol output, State next>
struct apply<Machine<Instructions, TapeLeft, TapeRight, state>, Instruction<state, input, output, Move::Right, next>> {
	using result = Machine<Instructions,
		typename append<output, TapeLeft>::tape,
		typename dropFirst<TapeRight>::tape,
		next>;
};

template <typename Instructions, typename TapeLeft, typename TapeRight, State state,
	Symbol input, Symbol output, Move move, State next>
struct apply<Machine<Instructions, TapeLeft, TapeRight, state>, Instruction<state, input, output, move, next>> {
};

template <typename Instructions, typename TapeLeft, typename TapeRight, State state>
struct Machine {
	using run = typename apply<Machine, typename find<Instructions, state, head<TapeRight>::value>::instruction>::result::run;
};

template <typename Instructions, typename TapeLeft, typename TapeRight>
struct Machine<Instructions, TapeLeft, TapeRight, State::Halt> {
	using run = Machine;
};

static void show(Tape<>) {
}

template <Symbol symbol, Symbol... symbols>
static void show(Tape<symbol, symbols...>) {
	std::cout << symbol;
	show(Tape<symbols...>());
}

template <typename Instructions, typename TapeLeft, typename TapeRight, State state>
static void show(Machine<Instructions, TapeLeft, TapeRight, state>) {
	show(TapeLeft());
	std::cout << '>';
	show(TapeRight());
	std::cout << std::endl;
}

template <typename Input>
using TTM = Machine<
	Instructions<
		Instruction<State::Init, 0, 1, Move::Left, State(1)>,
		Instruction<State(1), 0, 1, Move::Right, State(1)>,
		Instruction<State(1), 1, 0, Move::Right, State::Halt>
	>,
	EmptyTape<10>::tape,
	Input
>;

void test_ttm()
{

	show(TTM<EmptyTape<10>::tape>::run());
	show(TTM<Tape<0, 1, 1, 1>>::run());
#ifdef TTM_INPUT
	std::cout << "Your input: ";
	show(TTM<Tape<TTM_INPUT>>::run());
#endif

}

// constexpr Turing machine

#include <array>
#include <exception>

class CTM {
public:
	enum {
		States = 10,
		TapeLength = 20,
	};

	constexpr void parseInstruction(char const*& s)
	{
		s++;
	}

	constexpr void parse(char const*& s)
	{
		while(true)
			switch(s[0]) {
			case '(':
				parseInstruction(s);
				break;
			case '\0':
				return;
			default:
				// This will generate a compile error when
				// triggered during constexpr evaluation.
				throw std::runtime_error("Cannot parse instruction");
			}
	}

	constexpr CTM(char const* program)
		: m_program(program)
	{
		// "(state,input,output,move,next)(...)..."
		parse(program);
	}

	constexpr auto run(char const* input) const
	{
		std::array<char,TapeLength> tape{};

		for(size_t i = 0; input[i] && i < tape.size() - 1; i++)
			tape[i] = input[i];

		return tape;
	}

private:
	char const* m_program;
};

// Need a reference to a variable as template argument (can't use std::array as value).
template <std::array<char,CTM::TapeLength> const& tape>
static void show()
{
	std::cout << tape.data() << std::endl;
}

constexpr static CTM ctm{"("};
constexpr static auto tape = ctm.run("123");

static void test_ctm(char const* input)
{
	// tape must be known by the linker, so it must exist as global.
	show<tape>();

	if(input) {
		std::cout << "Your input: " << ctm.run(input).data() << std::endl;
	}
}


// main
int main(int argc, char** argv)
{
	test_ttm();
	test_ctm(argc >= 2 ? argv[1] : nullptr);
}

