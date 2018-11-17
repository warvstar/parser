#include "interpreter.h"
#include "Parser.h"
#include "helper.h"
#include "tokens.h"

Interpreter::Interpreter(Parsed& parsed) {
	auto parsed2 = Parsed::get();
	//UnparsedToken temp;
	//auto print = new FunctionToken(temp);
	//auto add = new FunctionToken(temp);
	//auto getInt = new FunctionToken(temp);
	//print->name = "print";
	//add->name = "add";
	//getInt->args["a"] = { "a", "int", "" };
	//parsed2->functions["print"] = print;
	std::string out_lua = "";
	auto tt = (ClassToken*)parsed2->root;
	for (auto t : tt->children) {
		t.second->GetLua(out_lua);
		out_lua += '\n';
	}
}