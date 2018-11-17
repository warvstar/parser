#include "tests.h"
#include "interpreter.h"
void InterpreterTest::Run(std::string_view source, Parsed& parsed) {
	auto interpreter = Interpreter(parsed);
}