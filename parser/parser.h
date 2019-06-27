#pragma once
#include "std.core.h"

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
typedef std::variant<uint8_t, uint16_t, uint32_t, uint64_t, int8_t, int16_t, int32_t, uint64_t, std::string> Variant;
class VM;
namespace Parser {
	void start();
	void end();
	bool getMachineCode(VM& vm, std::string_view source, bool _abs = false);
	void queueMachineCode(VM& vm, std::string_view source, bool _abs = false);
	bool preprocess(VM& vm, std::string_view source, std::vector<std::unique_ptr<Token>>& tokens, bool _abs = false);
	CPP tocpp(VM& vm, std::string_view source, bool _abs = false);
	CPP tocpp(VM& vm, std::vector<std::unique_ptr<Token>>& tokens, bool _abs = false);
	bool compile(VM& vm, std::string_view source, bool _abs = false);
	bool compile(VM& vm, std::vector<std::unique_ptr<Token>>& tokens, bool _abs = false);
	Variant execMachineCode(VM& vm, std::string_view source);
	Token* Parse(std::string_view source, std::vector<std::unique_ptr<Token>>& tokens);
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
	VM* vm;
	VM& getVM() {
		return *vm;
	};
	Parsed();
	std::unordered_map<std::string, struct FunctionToken*> functions;
	std::unordered_map<std::string, struct VariableToken*> variables;
};
