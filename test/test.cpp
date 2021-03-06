#include <iostream>
#include "parser.h"
#define SOL_LUAJIT 1
#include "sol.h"
int main() {
	sol::state lua;
	lua.open_libraries(sol::lib::base, sol::lib::ffi, sol::lib::jit, sol::lib::package);
	//auto script = Parser::getLua("../parser/gl.fe");
	auto script = Parser::getCode(Options::MACHINE_CODE, "../parser/test_0.fe");

	//import gl
	//in lua would generate
	//require('gl')
	//in c++ would generate, based on the exports the parser found
	//auto handle = dlopen("lib");
	//auto func = dlsym(handle, "func");


	/*try {
		lua.safe_script(script);
	}
	catch (std::exception &e) {
		luaL_error(lua, e.what());
	}*/
}
// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
