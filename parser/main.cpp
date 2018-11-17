#include "tests.h"

int main() {
	auto tests = Tests::Run();
	printf("Parser took:%f /ms\n", tests.parser.elapsed);
	printf("Interpeter took:%f /ms\n", tests.interp.elapsed);
	//printf("Jit took:%f /ms\n", tests.jit.elapsed);
	//printf("Lua took:%f /ms\n", tests.lua.elapsed);
	printf("LuaJIT took:%f /ms\n", tests.luajit.elapsed);
}