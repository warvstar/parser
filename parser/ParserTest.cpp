#include "tests.h"

void ParserTest::Run(std::string_view source, Parsed& parsed) {
	Parse(source);
}