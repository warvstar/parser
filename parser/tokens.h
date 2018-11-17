#pragma once
#include "std.core.h"
struct Arg {
	std::string name = "";
	std::string type = "";
	std::string value = "";
};
struct Func {
	std::vector<std::string> args;
};
enum class SymbolType {
	NA, Equals, Plus, Minus, ParenLeft, BracketLeft, ParenRight, BracketRight, Variable, Function, String, Number, Class, Colon, Declaration, Type, ArgsEnd, Comma, Extends, Int, UnsignedInt
};
struct Token {
	std::string name = "";
	std::string type = "";
	Token* parent = nullptr;
	uint16_t level = 0;
	std::vector<std::pair<SymbolType, std::variant<uint64_t, int64_t, Func, SymbolType, std::string, Token*>>> symbols;
	tsl::ordered_map<std::string, Token*> children;
	virtual void Print() {};
	virtual void GetLua(std::string& out_lua) {};
	virtual void GetCode(class VM& vm) {};
	virtual void GetCode(struct CodeBlock& block, class VM& vm) {};
	std::string GetInAnyScope(VM& vm, const std::string& var);
	std::string GetMangled();
	virtual void GetCodeBlock(std::vector<CodeBlock>& blocks, VM& vm) {};
	virtual void PreProcess(VM& vm) {};
};
struct ClassToken : public Token {
	std::string extends = "";
	ClassToken() {};
	ClassToken(std::vector<std::pair<SymbolType, std::variant<uint64_t, int64_t, Func, SymbolType, std::string, Token*>>>& symbols);
	void Print();
	void GetLua(std::string& out_lua);
	void GetCode(VM& vm);
	void GetCode(CodeBlock& block, VM& vm);
	void GetCodeBlock(std::vector<CodeBlock>& blocks, VM& vm);
	void PreProcess(VM& vm);
};
struct VariableToken : public Token {
	std::vector<std::string> value;
	VariableToken() {};
	VariableToken(std::vector<std::pair<SymbolType, std::variant<uint64_t, int64_t, Func, SymbolType, std::string, Token*>>>& symbols);
	void Print();
	void GetLua(std::string& out_lua);
	void GetCode(VM& vm);
	void GetCode(CodeBlock& block, VM& vm);
	void GetCodeBlock(std::vector<CodeBlock>& blocks, VM& vm);
	void PreProcess(VM& vm);
};
struct FunctionToken : public Token {
	tsl::ordered_map<std::string, Arg> args;
	//A list of variable tokens
	//Example
	//v = Compute()
	//name = v value = Compute()
	//v = 1
	//name = v value = 1
	//Compute()
	//name = 0 value = Compute()
	FunctionToken() {};
	FunctionToken(std::vector<std::pair<SymbolType, std::variant<uint64_t, int64_t, Func, SymbolType, std::string, Token*>>>& symbols);
	void Print();
	void GetLua(std::string& out_lua);
	void GetCode(VM& vm);
	void GetCode(CodeBlock& block, VM& vm);
	void GetCodeBlock(std::vector<CodeBlock>& blocks, VM& vm);
	void PreProcess(VM& vm);
};
