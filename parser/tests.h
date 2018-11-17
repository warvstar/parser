#pragma once
#include "helper.h"
#include "parser.h"
struct Test {
	std::chrono::system_clock::time_point start;
	std::chrono::system_clock::time_point end;
	std::chrono::system_clock::time_point elapsed;
	virtual void Run(std::string_view source, Parsed& parsed) {};
};
struct ParserTest : public Test {
	void Run(std::string_view source, Parsed& parsed) override;
};
struct InterpreterTest : public Test {
	void Run(std::string_view source, Parsed& parsed) override;
};
struct JitTest : public Test {
	void Run(std::string_view source, Parsed& parsed) override;
};
struct AotTest : public Test {
	void Run(std::string_view source, Parsed& parsed) override;
};
struct LuaTest : public Test {
	void Run(std::string_view source, Parsed& parsed) override;
};
struct LuaJITTest : public Test {
	void Run(std::string_view source, Parsed& parsed) override;
};
struct Tests {
	ParserTest parser; InterpreterTest interp; JitTest jit; AotTest aot; LuaTest lua; LuaJITTest luajit;
	std::array<Test*, 6> tests = {&parser, &interp, &jit, &aot, &lua, &luajit};
	static Tests Run() {
		std::string source = Helper::readFile("simple.fe");
		Tests tests;
		Parsed parsed;
		for (auto test : tests.tests)
			test->Run(source, parsed);
		return tests;
	};
};