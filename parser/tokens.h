#pragma once
#include "std.core.h"
enum class BasicType {
	nul, Bool, u8, u16, u32, u64, i8 = 8, i16 = 9, i32 = 10, i64 = 11, f16 = 14, f32 = 15, f64 = 16, str = 201, var = 202, variadic = 203, template_type = 204
};
enum class SymbolType {
	NA, Equals, Plus, Minus, ParenLeft, BracketLeft, ParenRight, BracketRight, Member, Function, Number, Class, Colon, Declaration, ArgsStart, ArgsEnd, BranchArgsEnd, Comma, Extends, Type, Variable, String, Char, I8, U8, I16, U16, I32, U32, I64, U64, Float, Double, Import, Branch, Else, While, Switch, Case, Default, EndBranch, Return, AngleBracketLeft,
	AngleBracketRight, False, True, Enum, Const, Star, ParamPack, CastAs, Any, TypeOf, ArrayStart, ArrayEnd, For, SizeOf
};
using Mixed = std::variant<uint64_t, int64_t, double, SymbolType, std::string, struct Label*, struct Token*>;
struct Type {
	BasicType type = BasicType::nul;
	std::string type_name = "";
	Mixed value = (uint64_t)0;
	bool isArray = false;
	bool isNativeArray = false;
	std::string GetType();
	std::string GetComplexType();
	std::string ToStringSafe() {
		std::string out = "";
		/*Helper::replaceAll(out, " ", "$");
		Helper::replaceAll(out, "*", "");
		Helper::replaceAll(out, "#", "$");*/
		return out;
	}
	std::string Serialize(Serializer& serializer) {
		serializer.Up();
		serializer.Add((int)type);
		serializer.Add(type_name);
		serializer.Add(value);
		serializer.Add(isArray);
		serializer.Add(isNativeArray);
		serializer.Down();
	};
	static Label Deserialize(const std::string& in2) {
		Label l;
		auto in = Helper::split(in2, "|");
		l.ptr = stoull(in[0]);
		l.name = in[1];
		l.debug = in[2];
		l.by = (By)stoi(in[3]);
		l.def = in[4];
		l.basic = (BasicType)stoi(in[5]);
		l.position = stoi(in[6]);
		return l;
	}
};
struct DemangledStruct {
	std::string name = "";
	Type type = {};
	std::string body = "";
	std::string mangled = "";
	std::vector<std::string> names;
	std::vector<class class_def*> args;
	int diff = 10;
	bool needs_compilation = false;
	bool is_pointer = false;
};
struct Arg {
	std::string name = "";
	std::string type = "";
	std::string value = "";
};
enum Status {
	CONTINUE, RESTART, EXIT
};
struct _func {
	std::string name = "";
	std::string ptr = "";
	std::string body = "";
};
struct CPP {
	std::string header = "";
	std::string in_out = "";
	std::unordered_map<std::string, _func> functions;
	std::unordered_map<std::string, std::string> classes;
	std::unordered_map<std::string, std::string> exports;

	CPP& Merge(CPP& other) {
		//functions.insert(functions.end(), other.functions.begin(), other.functions.end());
		header += other.header;
		in_out += other.in_out;
		functions.insert(other.functions.begin(), other.functions.end());
		classes.insert(other.classes.begin(), other.classes.end());
		exports.insert(other.exports.begin(), other.exports.end());
		return *this;
	}
	//CPP& operator+=(CPP& other) {
		//Merge(other);
	//}
	CPP& operator+=(CPP other) {
		return Merge(other);
	}
};
enum class THE_TOKEN_TYPE {TOKEN, VAR, FUNCTION, ENUM, CLASS};
struct Token {
	CPP out_cpp;
	THE_TOKEN_TYPE token_type = THE_TOKEN_TYPE::TOKEN;
	bool broken = false;
	bool needs_compile = true;
	bool needs_preprocess = true;
	bool _abs = false;
	bool global = false;
	std::string _body = "";
	std::string name = "";
	std::string mangled_name = "";
	Type type = { BasicType::nul, "", (uint64_t)0 };
	tsl::ordered_map<std::string, Type> args;
	tsl::ordered_map<std::string, Type> locals;
	Token* parent = nullptr;
	uint16_t level = 0;
	std::vector<std::pair<SymbolType, Mixed>> symbols; //todo: dont need to have this as a pair
	tsl::ordered_map<std::string, Token*> children;
	virtual void Print() {};
	virtual void GetCode(class VM& vm) {};
	virtual Status GetCPP(VM& vm) { return CONTINUE; };
	virtual void GetCode(struct CodeBlock& block, class VM& vm) {};
	std::string GetMangled();
	class_def* AddClass(VM& vm, std::string_view to_find, const std::vector<std::string>& tkns = {});
	void AddVariable(VM& vm, std::string_view type); //Fix, type should be const
	bool IsClassSet(VM& vm, std::string_view to_find);
	bool IsVarSet(VM & vm, std::string_view to_find);
	Type FindTypeForVariable(VM& vm, std::string& str);
	int SameType(VM& vm, class_def& a, SymbolType b, Mixed v, int& dif, bool& needs_compilation);
	Status FindQualified(VM& vm, std::vector<DemangledStruct>& demangled, SymbolType type, Mixed v, uint8_t index, const std::string& name);
	bool FindMostQualified(std::vector<std::pair<SymbolType, Mixed>>& symbols, uint32_t index, std::string& name, DemangledStruct& out, Status& restart, bool can_restart = true);
	Type mixedToType2(VM& vm, std::pair<SymbolType, Mixed>& v);
	virtual void GetCodeBlock(std::vector<CodeBlock>& blocks, VM& vm) {};
	virtual Status PreProcess(VM& vm) { return CONTINUE; };
	virtual const THE_TOKEN_TYPE GetTokenType() { return THE_TOKEN_TYPE::TOKEN; };
};
struct ClassToken : public Token {
	std::string extends = "";
	ClassToken() {};
	ClassToken(std::vector<std::pair<SymbolType, Mixed>>& symbols);
	void Print();
	Status GetCPP(VM& vm);
	Status PreProcess(VM& vm);
	const THE_TOKEN_TYPE GetTokenType() { return THE_TOKEN_TYPE::CLASS; };
};
struct VariableToken : public Token {
	std::vector<std::string> value;
	VariableToken() {};
	VariableToken(std::vector<std::pair<SymbolType, Mixed>>& symbols);
	void Print();
	Status GetCPP(VM& vm);
	bool GetName();
	bool ProcessArgs(VM & vm, DemangledStruct & in_struct);
	Status PreProcess(VM& vm);
	const THE_TOKEN_TYPE GetTokenType() { return THE_TOKEN_TYPE::VAR; };
};
struct EnumToken : public Token {
	std::vector<std::string> value;
	EnumToken() {};
	EnumToken(std::vector<std::pair<SymbolType, Mixed>>& symbols);
	void Print();
	Status GetCPP(VM& vm);
	Status PreProcess(VM& vm);
	const THE_TOKEN_TYPE GetTokenType() { return THE_TOKEN_TYPE::ENUM; };
};
struct FunctionToken : public Token {
	bool is_virtual = false;
	bool importing_func = false;
	bool is_template = false;
	std::string types = "";
	FunctionToken() {};
	FunctionToken(std::vector<std::pair<SymbolType, Mixed>>& symbols);
	void Print();
	std::string MangleFunction(VM & vm);
	Status GetCPP(VM& vm);
	Status PreProcess(VM& vm);
	const THE_TOKEN_TYPE GetTokenType() { return THE_TOKEN_TYPE::FUNCTION; };
};
