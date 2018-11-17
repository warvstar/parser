#pragma once
#include "std.core.h"
#include "bitmask.hpp"
#include <mutex>
#include "tokens.h"
/*enum class Options : uint32_t {
	CPP = 1,
	LUA = 2,
	MACHINE_CODE = 3
};*/
enum class Options {
	CPP = 0x01,
	LUA = 0x02,
	MACHINE_CODE = 0x04,

	// 2. Define max bitmask value
	_bitmask_max_element = MACHINE_CODE
};
BITMASK_DEFINE(Options);
/*template<typename Enum>
struct EnableBitMaskOperators
{
	static const bool enable = false;
};

template<typename Enum>
typename std::enable_if<EnableBitMaskOperators<Enum>::enable, Enum>::type
operator |(Enum lhs, Enum rhs) {
	using underlying = typename std::underlying_type<Enum>::type;
	return static_cast<Enum> (static_cast<underlying>(lhs) | static_cast<underlying>(rhs));
}
template<>
struct EnableBitMaskOperators<Options>
{
	static const bool enable = true;
};
constexpr enum Options operator |(const enum Options selfValue, const enum Options inValue) {
	return (enum Options)(uint32_t(selfValue) | uint32_t(inValue));
}
Options& operator &=(Options &lhs, Options rhs) {
	lhs = static_cast<Options> ( static_cast<std::underlying_type<Options>::type>(lhs) & static_cast<std::underlying_type<Options>::type>(rhs));
	return lhs;
}
inline constexpr Options operator&(Options a, Options b) {
	return a = static_cast<Options> (a & b);
}
constexpr enum Options operator &(const enum Options lhs, const enum Options rhs) {
	return static_cast<Options> ( static_cast<std::underlying_type<Options>::type>(lhs) & static_cast<std::underlying_type<Options>::type>(rhs));
}*/
struct ParsedResult {
	std::string cpp = "";
	std::string lua = "";
	std::string machine_code = "";
};
namespace Parser {
	ParsedResult getCode(const Options flags, const std::string& fn);
	std::string getLua(std::string_view source);
	std::string getCpp(std::string_view source);
	std::string getMachineCode(std::string_view source);
}
enum TokenType {
	NA, CLASS, VARIABLE, FUNCTION
};
/*struct UnparsedToken {
	int32_t starting_spaces = 0;
	int index = 0;
	std::string name = "";
	std::string type = "";
	//SymbolType type = SymbolType::NA;
	UnparsedToken* parent = nullptr;
	std::vector<UnparsedToken> children;
	std::vector<Arg> args;
	std::vector<std::pair<SymbolType, std::variant<uint64_t, int64_t, Func, std::string, UnparsedToken*>>> values = { "", "" };
};*/
//struct Token* ParseToken(UnparsedToken& token);
struct Parsed {
	static std::unique_ptr<Parsed> instance;
	static std::once_flag onceFlag;
	static Parsed* get() {
		std::call_once(Parsed::onceFlag, []() {
			instance.reset(new Parsed);
		});
		return (instance.get());
	}
	Token* root = nullptr;
	std::vector<std::unique_ptr<Token>> tokens;
	std::unordered_map<std::string, struct FunctionToken*> functions;
	std::unordered_map<std::string, struct VariableToken*> variables;
};
//change everything to string_view later and use indexes for start and end of token
struct Variable {
	std::string name = "";
	std::string type = "";
};
struct Function {
	std::string name = "";
	std::string ret = "";
};
struct Class {
	std::string name = "";
	std::vector<Variable> variables;
	std::vector<Function> functions;

};
struct Unparsed {
	uint32_t start, end = 0;
};
struct UnparsedVariable : public Unparsed {
	Function ToVariable() {
	};
};
struct UnparsedFunction {
	Function ToFunction() {
	};
};
struct UnparsedClass {
	Function ToClass() {
	};
};
void Parse(std::string_view source);