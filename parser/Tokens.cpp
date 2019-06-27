#include "tokens.h"
#include "parser.h"
#include "helper.h"
#include "fmt/format.h"
#include "vm.h"
#include "basic_types.h"
using namespace fmt;
#define class_template "\
local {0}\n\
local _class_0\n\
local _base_0 = {4}\n\
_base_0.__index = _base_0\n\
_class_0 = setmetatable({2}\n\
__init = function() end,\n\
__base = _base_0,\n\
__name = \"{0}\"\n\
{3}, {2}\n\
__index = _base_0,\n\
__call = function(cls, ...)\n\
local _self_0 = setmetatable({1}, _base_0)\n\
cls.__init(_self_0, ...)\n\
return _self_0\n\
end\n\
{3})\n\
_base_0.__class = _class_0\n\
{0} = _class_0\n"
#define class_extends_template "\
local {0}\n\
local _class_0\n\
local _parent_0 = {1}\n\
local _base_0 = {5}\n\
_base_0.__index = _base_0\n\
setmetatable(_base_0, _parent_0.__base)\n\
_class_0 = setmetatable({3}\n\
__init = function(self, ...)\n\
return _class_0.__parent.__init(self, ...)\n\
end,\n\
__base = _base_0,\n\
__name = \"{0}\",\n\
__parent = _parent_0\n\
{4}, {3}\n\
	__index = function(cls, name)\n\
	local val = rawget(_base_0, name)\n\
	if val == nil then\n\
		local parent = rawget(cls, \"__parent\")\n\
		if parent then\n\
		return parent[name]\n\
		end\n\
	else\n\
		return val\n\
	end\n\
	end,\n\
	__call = function(cls, ...)\n\
	local _self_0 = setmetatable({2}, _base_0)\n\
	cls.__init(_self_0, ...)\n\
	return _self_0\n\
	end\n\
{4})\n\
_base_0.__class = _class_0\n\
if _parent_0.__inherited then\n\
	_parent_0.__inherited(_parent_0, _class_0)\n\
	end\n\
	{0} = _class_0\n"
ClassToken::ClassToken(std::vector<std::pair<SymbolType, Mixed>>& symbols) {
	token_type = THE_TOKEN_TYPE::CLASS;
	symbols.erase(symbols.begin());
	name = std::get<std::string>(symbols[0].second);
	if (symbols.size() > 1)
		extends = std::get<std::string>(symbols[2].second);
	/*level = token.starting_spaces;
	name = token.values[3];
	if (token.values.size() > 4)
		extends = token.values[5];
	for (auto c : token.children) {
		auto t = ParseToken(c);
		t->parent = this;
		if (t->name.size() > 0)
			children[t->name] = t;
	}*/
}
FunctionToken::FunctionToken(std::vector<std::pair<SymbolType, Mixed>>& symbols) {
	token_type = THE_TOKEN_TYPE::FUNCTION;
	name = std::get<std::string>(symbols[0].second);
	symbols.erase(symbols.begin());
	for (auto sym : symbols) {
		//sym.first
	}
	this->symbols = symbols;
	/*level = token.starting_spaces;
	name = token.values[0];
	if (name.size() > 0)
		Parsed::get()->functions[name] = this;
	type = token.values[1];
	//args = token.args;
	//args.erase(std::remove_if(args.begin(),	args.end(),	[](Arg x) {return x.name == "(" || x.name == ")" || x.name == ","; }), args.end());
	for (auto a : token.args) {
		if (a.name == "(" || a.name == ")" || a.name == ",") {

		}
		else
			args[a.name] = a;
	}
	for (auto c : token.children) {
		auto t = (VariableToken*)ParseToken(c);
		t->parent = this;
		if (t->name.size() > 0)
			children[t->name] = t;
		else {
			std::string n = "|";
			for (int i = 0; i < children.size(); ++i)
				n += '|';
			children[n] = t;
		}
	}*/
}
bool Helper::BasicTypeCheck(const std::string& type) {
	if (type == "u8" || type == "i8" || type == "u16" || type == "i16" || type == "u32" || type == "i32" || type == "u64" || type == "i64" || type == "f32" || type == "f64") //should add void here
		return true;
	else
		return false;
}
bool Helper::BasicTypeCheck(const Type& type) {
	if (type.type == BasicType::u8 || type.type == BasicType::i8 || type.type == BasicType::u16 || type.type == BasicType::i16 || type.type == BasicType::u32 || type.type == BasicType::i32 || type.type == BasicType::u64 || type.type == BasicType::i64 || type.type == BasicType::f32 || type.type == BasicType::f64 || type.type == BasicType::nul) //should add void here
		return true;
	else
		if (type.type_name == "void")
			return true;
		else
			return false;
}
int na_counter = 0;
BasicType StringToBasic(const std::string& str) {
	if (str.size() == 0)
		return BasicType::nul;
	else if (str == "u8")
		return BasicType::u8;
	else if (str == "u16")
		return BasicType::u16;
	else if (str == "u32")
		return BasicType::u32;
	else if (str == "u64")
		return BasicType::u64;
	else if (str == "i8")
		return BasicType::i8;
	else if (str == "i16")
		return BasicType::i16;
	else if (str == "i32")
		return BasicType::i32;
	else if (str == "i64")
		return BasicType::i64;
	else if (str == "f32")
		return BasicType::f32;
	else if (str == "f64")
		return BasicType::f64;
	else if (str == "str")
		return BasicType::str;
	else if (str == "void")
		return BasicType::nul;
	return BasicType::var;
}
BasicType SymbolToBasic(const SymbolType& symbol) {
	return BasicType::nul;
}
std::string SymbolToString(const SymbolType& symbol) {
	if (symbol == SymbolType::U8)
		return "u8";
	else if (symbol == SymbolType::U16)
		return "u16";
	else if (symbol == SymbolType::U32)
		return "u32";
	else if (symbol == SymbolType::U64)
		return "u64";
	else if (symbol == SymbolType::I8)
		return "i8";
	else if (symbol == SymbolType::I16)
		return "i16";
	else if (symbol == SymbolType::I32)
		return "i32";
	else if (symbol == SymbolType::I64)
		return "i64";
	else if (symbol == SymbolType::Float)
		return "f32";
	else if (symbol == SymbolType::Double)
		return "f64";
	else if (symbol == SymbolType::String)
		return "str";
	return "";
}
Type SymbolToType(const SymbolType& symbol) {
	if (symbol == SymbolType::U8)
		return { BasicType::u8, "u8" };
	else if (symbol == SymbolType::U16)
		return { BasicType::u16, "u16" };
	else if (symbol == SymbolType::U32)
		return { BasicType::u32, "u32" };
	else if (symbol == SymbolType::U64)
		return { BasicType::u64, "u64" };
	else if (symbol == SymbolType::I8)
		return { BasicType::i8, "i8" };
	else if (symbol == SymbolType::I16)
		return { BasicType::i16, "i16" };
	else if (symbol == SymbolType::I32)
		return { BasicType::i32, "i32" };
	else if (symbol == SymbolType::I64)
		return { BasicType::i64, "i64" };
	else if (symbol == SymbolType::Float)
		return { BasicType::f32, "f32" };
	else if (symbol == SymbolType::Double)
		return { BasicType::f64, "f64" };
	else if (symbol == SymbolType::String)
		return { BasicType::str, "str" };
	return { BasicType::var, "" };
}
const SymbolType& StringToSymbol(const std::string& str) {
	if (str == "u8")
		return SymbolType::U8;
	else if (str == "u16")
		return SymbolType::U16;
	else if (str == "u32")
		return SymbolType::U32;
	else if (str == "u64")
		return SymbolType::U64;
	else if (str == "i8")
		return SymbolType::I8;
	else if (str == "i16")
		return SymbolType::I16;
	else if (str == "i32")
		return SymbolType::I32;
	else if (str == "i64")
		return SymbolType::I64;
	else if (str == "f32")
		return SymbolType::Float;
	else if (str == "f64")
		return SymbolType::Double;
	else if (str == "str")
		return SymbolType::String;
	return SymbolType::NA;
}
Type StringToType(const std::string& str) {
	if (str == "u8")
		return {BasicType::u8, str};
	else if (str == "u16")
		return { BasicType::u16, str };
	else if (str == "u32")
		return { BasicType::u32, str };
	else if (str == "u64")
		return { BasicType::u64, str };
	else if (str == "i8")
		return { BasicType::i8, str };
	else if (str == "i16")
		return { BasicType::i16, str };
	else if (str == "i32")
		return { BasicType::i32, str };
	else if (str == "i64")
		return { BasicType::i64, str };
	else if (str == "f32")
		return { BasicType::f32, str };
	else if (str == "f64")
		return { BasicType::f64, str };
	else if (str == "str")
		return { BasicType::str, str };
	return { BasicType::var, str, (uint64_t)0 };
}
std::string MixedToString(const Mixed& v) {
	if (std::holds_alternative<uint64_t>(v))
		return std::to_string(std::get<uint64_t>(v));
	else if (std::holds_alternative<int64_t>(v))
		return std::to_string(std::get<int64_t>(v));
	else if (std::holds_alternative<double>(v))
		return std::to_string(std::get<double>(v));
	else if (std::holds_alternative<std::string>(v))
		return std::get<std::string>(v); //was i8*
}
Status CreateClass(VM& vm, const std::string& name2) {
	if ((int)name2.find('$') > -1) {
		auto split = Helper::split_1(name2, '$');
		auto var_method = split[0];
		auto var_template = split[1];
		if (auto _class = vm.FindClass(name2)) {
			//get
		}
		else if (auto _class = vm.FindClass(var_method)) {
			Parser::compile(vm, "class vec$u8\n size:u64\n capacity:u64\n data:u8*\n resize(v:u8):void:\n  size = v\n  data = allocc(v * sizeof(u8))\n");
			return RESTART;
		}
		else {
			//error?
		}
	}
}
VariableToken::VariableToken(std::vector<std::pair<SymbolType, Mixed>>& symbols) {
	symbols.erase(std::remove_if(symbols.begin(), symbols.end(), [](std::pair<SymbolType, Mixed> v) {return v.first == SymbolType::ArgsStart; }), symbols.end());
	if (symbols[0].first == SymbolType::Variable) {
		name = std::get<std::string>(symbols[0].second);
		int found_period = name.find('.');
		std::string method_sig = "";
		if (found_period > -1) {
			//get mangled function name
			auto split = Helper::split_1(name, '.');
			name = split[0];
			symbols[0].first = SymbolType::Member;
			symbols[0].second = split[1];
		}
		else {
			symbols.erase(symbols.begin());
		}
	}
	if (name.size() != 0) {
		if (symbols.size() > 1)
			if (symbols[1].first == SymbolType::Type) {
				
				type = { StringToBasic(std::get<std::string>(symbols[1].second)), std::get<std::string>(symbols[1].second) };
			}
		//name = std::to_string(na_counter++);
	}
	
	for (uint16_t i = 0; i < symbols.size(); ++i) {
		for (uint16_t i = 0; i < symbols.size(); ++i) {
			auto& v = symbols[i];
			if (v.first == SymbolType::Function && symbols.size() > i + 2) {
				auto& v2 = symbols[i + 1];
				if (v2.first == SymbolType::AngleBracketLeft) {
					auto& v3 = symbols[i + 2];
					auto& v4 = symbols[i + 3];
					auto old = std::get<std::string>(v.second);
					v.second = std::get<std::string>(v.second) + '$' + std::get<std::string>(v3.second);
					auto _new = std::get<std::string>(v.second);
					i += 2;
					v2.first = SymbolType::NA;
					v3.first = SymbolType::NA;
					v4.first = SymbolType::NA;
					//name = _new;
					//move this to preprocess
					//Get or build the templated class
					/*auto& vm = Parsed::get()->getVM();
					if (auto _class = vm.FindClass(_new)) {
						//get
					}
					else if (auto _class = vm.FindClass(old)) {
						//build
						//std::vector<std::unique_ptr<Token>> tokens;
						//Parser::Parse("class vec$u8\n size:u64\n capacity:u64\n data:u8\n", tokens);
						Parser::compile(vm, "class vec$u8\n size:u64\n capacity:u64\n data:u8\n func5:\n  data\n");
						/*auto _newclass = _class->BuildClassFromTemplate(vm, std::get<std::string>(v3.second));
						std::vector<std::pair<SymbolType, Mixed>> syms = { {SymbolType::Function, name} };
						children[name] = new FunctionToken(syms);
						//children[name]->type = { BasicType::var, name };
						children[name]->PreProcess(vm);
						auto got_func = vm.labels.find(children[name]->mangled_name);
						if (got_func == vm.labels.end()) {
							printf("Function not created?\n");
						}
						_newclass->AddFunction(vm, Func{ name, &got_func->second, false });*/
					/*}
					else {
						//error?
					}*/

				}
			}
		}
	}
	symbols.erase(std::remove_if(symbols.begin(), symbols.end(), [](std::pair<SymbolType, Mixed> v) {return v.first == SymbolType::NA; }), symbols.end());

	//if (isdigit(name.front())) {
		//name = "na_" + std::to_string(na_counter++);
	//}
	this->symbols = symbols;
	/*level = token.starting_spaces;
	name = token.name;
	if (name.size() > 0)
		Parsed::get()->variables[name] = this;
	type = token.type;
	if (token.values.size() > 2)
		for (int i = 2; i < token.values.size(); ++i)
			value.push_back(token.values[i]);*/
}
void ClassToken::Print() {
	std::string n = name;
	if (extends.size() > 0)
		n += " extends " + extends;
	printf("Class:%s\n", n.data());
	for (auto c : children) {
		printf("%s", Helper::GetSpaces(c.second->level).data());
		c.second->Print();
	}
}
void FunctionToken::Print() {
	printf("Function:%s\n", name.data());
	for (auto c : children) {
		printf("%s", Helper::GetSpaces(c.second->level).data());
		c.second->Print();
	}
}
void VariableToken::Print() {
	printf("Variable:%s\n", name.data());
}
std::string Token::GetMangled() {
	std::string out = "";
	if (parent)
		out += parent->GetMangled();
	if (name != "root")
		out += name;
	return out;
}
//move this somewhere better and add any small types to it, including child classes
std::set<std::string> is_small = { "i32", "u32" };
bool isSmall(const std::string& s) {
	auto got = is_small.find(s);
	if (got != is_small.end()) {
		return true;
	}
	return false;
}
Status ClassToken::PreProcess(VM& vm) {
	auto& variadic_def = vm.class_defs[name];
	variadic_def.name = name;
	variadic_def.extends = extends;
	//variadic_def.basic = BasicType::var;
	vm.reserved_keywords[name] = SymbolType::Type;
	std::string temp;
	bool has_constructor = false;
	//std::string tt0 = "TABLE(" + name + ")";
	//std::string tt = "GET" + tt0;
	//variadic_def.variables[tt] = Type{ BasicType::u64, "VTABLE", tt0 };
	//get heirachy and create vtable, will be able to use smaller type, u8 for most relationships.
	if (extends.size() > 0) {
		std::vector<class_def*> heirachy;
		heirachy.push_back(&variadic_def);
		auto t = vm.FindClass(name);
		while (auto v = t->GetParentClass(vm)) {
			heirachy.push_back(v);
			t = v;
		}
		for (int i = heirachy.size() - 1; i > -1; --i)
			heirachy[i]->variables["VTABLE"] = Type{ BasicType::u8, "u8", abs((int64_t)(i+1-heirachy.size())) };

	}
	//alternitavely can use a global id based on the position in the map
	//variadic_def.variables["VTABLE"] = Type{ BasicType::u64, "u64", (uint64_t)std::distance(vm.class_defs.begin(),vm.class_defs.find(name)) };
	for (auto c : children) {
		auto old_name = c.second->name;
		if (name == old_name)
			children[name]->type = { BasicType::var, name };
		if (c.second->type.type_name.size() > 0) {
			if (c.second->type.type_name.front() == '$') {
				c.second->type = { BasicType::template_type };
				variadic_def.is_template = true;
			}
			else if (int(c.second->type.type_name.find('$')) > 0) {
				CreateClass(vm, c.second->type.type_name);
			}
		}
		c.second->PreProcess(vm);
		temp += ",\n";
		//std::pair<std::string, std::string> mixedToType2(VM& vm, FunctionToken* parent, const std::pair<SymbolType, Mixed>& v);
		if (c.second->GetTokenType() == THE_TOKEN_TYPE::VAR) //change to process all vars first, then do functions, to avoid functions not seeing variables.
			variadic_def.variables[c.second->name] = { c.second->type };
		else if (c.second->GetTokenType() == THE_TOKEN_TYPE::FUNCTION) {
			std::string args = "";
			if (c.second->args.size() > 1) {
				args += '(';
				int i = 0;
				for (auto a : c.second->args) {
					if (i++ == 0) continue;
					args += a.second.GetType() + ",";
				}
				args.pop_back();
				args += "):";
			}
			auto got_func = vm.labels.find(c.second->mangled_name);
			if (got_func == vm.labels.end()) {
				printf("Function not created?\n");
				continue;
			}
			variadic_def.AddFunction(vm, Func{ old_name, &got_func->second, false, c.second->_body });

			if (old_name == name)
				has_constructor = true;
		}
	}
	if (!has_constructor/* && !variadic_def.is_template*/) {
		std::vector<std::pair<SymbolType, Mixed>> syms = { {SymbolType::Function, name} /*, {SymbolType::Colon, ""}, {SymbolType::Variable, "T$"}, {SymbolType::Colon, ":"}, {SymbolType::Type, "Animal"}, {SymbolType::ArgsEnd, ")"}, {SymbolType::Colon, ":"}, {SymbolType::Type, "Animal"}, {SymbolType::Colon, ":"}*/ };
		children[name] = new FunctionToken(syms);
		children[name]->type = {BasicType::var, name};
		children[name]->PreProcess(vm);
		auto got_func = vm.labels.find(children[name]->mangled_name);
		if (got_func == vm.labels.end()) {
			printf("Function not created?\n");
		}
		variadic_def.AddFunction(vm, Func{ name, &got_func->second, false, children[name]->_body });
	}
	if (variadic_def.is_template) {
		for (auto c : children)
			c.second->needs_compile = false;
		//if class is templated, then make sure all functions get a templated function
		for (auto f : variadic_def.functions) {
			/*std::vector<std::string> args2;
			for (auto arg1 : args)
				args2.push_back(arg1.first);
			vm.addTemplateFunction(name, mangled_name, _body, args2);*/
		}
	}

	

	//for (auto c : children) {
		//c.second->PreProcess(vm);
	//}
	return CONTINUE;
}
//outputs a list of mangled names starting from the most likely to least
//so that if we call a function add2(1,1) it will first look for add2(u8,u8) then add2(u16, u16)
//or even better, find name in function map and then we know the possible candidates

std::vector<std::string> split_1_keepall(std::string_view s, const char c) {
	std::string buff{ "" };
	std::vector<std::string> v;

	for (int i = 0; i < s.size(); ++i) {
		auto n = s[i];
		if (n != c) buff += n; else {
			v.push_back(buff);
			buff = "";
		}
	}
	if (buff != "") v.push_back(buff);

	return v;
};
FunctionToken* GetFunction(Token* token) {
	if (!token)
		return nullptr;
	if (token->token_type == THE_TOKEN_TYPE::FUNCTION)
		return (FunctionToken*)token;
	else return GetFunction(token->parent);
}
DemangledStruct Helper::Demangle(const std::string& str, std::vector<std::string> vec, const std::string& body) {
	DemangledStruct out;
	out.names = vec;
	out.body = body;
	out.mangled = str;
	auto tmp = str;
	if (tmp.back() == '$' && tmp[tmp.size()-2] != '_')
		tmp.pop_back();
	auto split = split_1_keepall(tmp, '_');
	if (split.size() < 2)
		return out;
	out.type = StringToType(split[1]);
	out.name = split[2];
	split.erase(split.begin(), split.begin() + 3);
	auto& vm = Parsed::get()->getVM();
	for (auto arg : split) {
		if (auto got = vm.FindClass(arg))
			out.args.push_back(got);
		else
			printf("Unknown type (%s).\n", arg.data());
	}
	return out;
}
template <std::size_t ... Is>
std::string my_format(const std::string& format, const std::vector<std::string>& v, std::index_sequence<Is...>) { return fmt::format(format, v[Is]...); }
template <std::size_t N>
std::string my_format(const std::string& format, const std::vector<std::string>& v) { return my_format(format, v, std::make_index_sequence<N>()); }
std::string mixedToType(const Mixed& v) {
	if (std::holds_alternative<uint64_t>(v))
		return "u64";
	else if (std::holds_alternative<int64_t>(v))
		return "i64";
	else if (std::holds_alternative<std::string>(v))
		return "u64"; //was i8*
}
bool has_only_digits(const std::string s) {
	return s.find_first_not_of("0123456789.") == std::string::npos;
}
Token* GetClassOrFunction(Token* token) {
	if (!token)
		return nullptr;
	if (token->token_type == THE_TOKEN_TYPE::FUNCTION || token->token_type == THE_TOKEN_TYPE::CLASS)
		return token;
	else return GetClassOrFunction(token->parent);
}
bool Token::IsClassSet(VM& vm, std::string_view type) {
	if (type == "$") return true;
	auto is_set = out_cpp.classes.find(type.data());
	if (is_set != out_cpp.classes.end())
		return true;
	return false;
}
bool Token::IsVarSet(VM& vm, std::string_view type) {
	auto _parent = GetFunction(parent);
	if (!_parent) _parent = (FunctionToken*)parent;
	if (_parent) {
		auto got0 = _parent->locals.find(type.data());
		if (got0 != _parent->locals.end()) {
			//return true;
		}
		auto got = _parent->args.find(type.data());
		if (got != _parent->args.end()) {
			return true;
		}
	}
	auto got1 = out_cpp.exports.find(type.data());
	if (got1 != out_cpp.exports.end())
		return true;
	return false;
}
void Token::AddVariable(VM& vm, std::string_view type) {
	if (IsVarSet(vm, type))
		return;
	auto got = vm.labels.find(type.data());
	if (got != vm.labels.end()) {
		out_cpp.exports[type.data()] = got->second.def;
		auto split = Helper::split_1(type.data(), '_');
		if (split.size() > 0) {
			for (auto f : split)
				IsClassSet(vm, f);
		}
	}
}
Type FindTypeForString(VM& vm, std::string str) {
	auto got2 = vm.macros.find(str);
	if (got2 != vm.macros.end()) {
		str = got2->second;
		if (str.size() > 1) {
			if (str.substr(0, 2) == "0x") {
				int i_auto = std::stoll(str, nullptr, 0);
				str = std::to_string(i_auto);
			}
		}
	}
	if (str.front() == '-') {
		auto str2 = str.substr(1);
		if (has_only_digits(str2)) {
			if (int(str2.find('.')) > -1) {
				auto val = std::stod(str2.data());
				if (val > FLT_MAX)
					return { BasicType::f64, "f64" };
				else
					return { BasicType::f32, "f32" };
			}
			else {
				auto val = std::stoll(str2.data());
				if (val > INT32_MAX)
					return { BasicType::i64, "i64" };
				else if (val > INT16_MAX)
					return { BasicType::i32, "i32" };
				else if (val > INT8_MAX)
					return { BasicType::i16, "i16" };
				else
					return { BasicType::i8, "i8" };
			}
		}
	}
	if (has_only_digits(str)) {
		if (int(str.find('.')) > -1) {
			auto val = std::stod(str.data());
			if (val > FLT_MAX)
				return { BasicType::f64, "f64" };
			else
				return { BasicType::f32, "f32" };
		}
		else {
			auto val = std::stoull(str.data());
			if (val > UINT32_MAX)
				return { BasicType::u64, "u64" };
			else if (val > UINT16_MAX)
				return { BasicType::u32, "u32" };
			else if (val > UINT8_MAX)
				return { BasicType::u16, "u16" };
			else
				return { BasicType::u8, "u8" };
		}
	}
	return {};
}
struct Variable {
	std::string def = "";
	Type type;
};
/*Variable FindVariableForStr(VM& vm, FunctionToken* _parent, const std::string& str) {
	auto split = Helper::split_1(str, '.');
	std::string left = "";
	std::string right = "";
	if (split.size() > 1) {
		left = split[0];
		right = split[1];
		int found_left_bracket = right.find('[');
		if (found_left_bracket > 0)
			right = right.substr(0, found_left_bracket);
	}
	else
		left = str;

}*/
Type Token::FindTypeForVariable(VM& vm, std::string& str) {
	auto _parent = GetFunction(parent);
	if (!_parent) _parent = (FunctionToken*)parent;
	//Make sure parent is function?
	auto type = FindTypeForString(vm, str);
	if (type.type != BasicType::nul)
		return type;

	if (str == "_" || str == "#")
		return { BasicType::template_type, "$" };
	auto split = Helper::split_1(str, '.');
	std::string left = "";
	std::string right = "";
	if (split.size() > 1) {
		left = split[0];
		right = split[1];
		int found_left_bracket = right.find('[');
		if (found_left_bracket > 0)
			right = right.substr(0, found_left_bracket);
	}
	else
		left = str;

	if (auto got2 = vm.FindClass(left)) {
		if (right.size() > 0) {
			if (auto got3 = got2->FindVariable(vm, right))
				return *got3;
		} else
			return StringToType(got2->name);
	}
	auto got = _parent->locals.find(left);
	if (got != _parent->locals.end())
		if (right.size() > 0) {
			Type tt = got->second;
			if (auto got2 = vm.FindClass(tt.GetType())) {
				if (auto got3 = got2->FindVariable(vm, right))
					return *got3;
			}
		}
		else
			return got->second;
	else {
		auto got = _parent->args.find(left);
		if (got != _parent->args.end()) {
			if (right.size() > 0) {
				if (auto got2 = vm.FindClass(got->second.type_name)) {
					if (auto got3 = got2->FindVariable(vm, right))
						return *got3;
				}
			}
			else
				return got->second;
		}
		else {
			auto parent = (Token*)_parent;
			while (parent) {
				if (auto got4 = vm.FindClass(parent->name)) {
					//check if class property
					if (auto got5 = got4->FindVariable(vm, right)) {
						str = "T$->" + str;
						return *got5;
					}
				}
				parent = parent->parent;
			}
			{
				auto got = vm.labels.find(left);
				if (got != vm.labels.end()) {
					if (right.size() == 0) {
						return got->second.GetType();
					}
					else {
						if (auto got2 = vm.FindClass(got->second.GetType().type_name)) {
							if (auto got3 = got2->FindVariable(vm, right)) {
								AddVariable(vm, left);
								AddClass(vm, got->second.GetType().type_name);
								return *got3;
							}
							else {
								printf("Var %s does not exist for class %s.\n", right.data(), got->second.GetType().type_name.data());
							}
						}
						else
							printf("ERROR21\n");
					}
				}
				else {
					auto got = vm.functions.find(left);
					if (got != vm.functions.end()) {
						if (got->second.size() > 1) {
							printf("Too many versions of %s, pick one ->\n", left.data());
							for (auto f : got->second)
								printf("  %s\n", f->name.data());
						}
						else if (got->second.size() == 1) {
							return { BasicType::u64, "u64" };
						}
						else {
							printf("Could'nt find variable %s for mangle function(%s).\n", left.data(), right.data());
						}
					}
					else {

					}

				}
			}
		}
	}
	return {BasicType::nul};
}
bool CheckIfTypeFits(VM& vm, class_def& a, Type type) {
	if (a.name == type.type_name)
		return true;
	if (Helper::BasicTypeCheck(a.name)) { //hack, fix this to work on integers
		if (Helper::BasicTypeCheck(type))
			return true;
	}
	if (auto c = vm.FindClass(type.type_name)) {
		if (c->HasParent(vm, a.name))
			return true;
	}
	if (auto c = vm.FindClass(a.name)) {
		if (c->HasParent(vm, type.type_name))
			return true;
	}
	return false;
}
int Token::SameType(VM& vm, class_def& a, SymbolType b, Mixed v, int& dif, bool& needs_compilation) {
	if (a.basic == BasicType::template_type) {
		dif = 100;
		needs_compilation = true;
		std::string type = SymbolToString(b);
		if (type.size() == 0 && b == SymbolType::Variable) {
			auto val = std::get<std::string>(v);
			type = FindTypeForVariable(vm, val).type_name;
		}
		a.name = type;
		return true;
	}
	if (b == SymbolType::U8) {
		dif = (int)BasicType::u8 - (int)a.basic;
	}
	else if (b == SymbolType::U16) {
		dif = (int)BasicType::u16 - (int)a.basic;
	}
	else if (b == SymbolType::U32) {
		dif = (int)BasicType::u32 - (int)a.basic;
	}
	else if (b == SymbolType::U64) {
		dif = (int)BasicType::u64 - (int)a.basic;
	}
	else if (b == SymbolType::I8) {
		dif = (int)BasicType::i8 - (int)a.basic;
	}
	else if (b == SymbolType::I16) {
		dif = (int)BasicType::i16 - (int)a.basic;
	}
	else if (b == SymbolType::I32) {
		dif = (int)BasicType::i32 - (int)a.basic;
	}
	else if (b == SymbolType::I64) {
		dif = (int)BasicType::i64 - (int)a.basic;
	}
	else if (b == SymbolType::Float) {
		dif = (int)BasicType::f32 - (int)a.basic;
	}
	else if (b == SymbolType::Double) {
		dif = (int)BasicType::f64 - (int)a.basic;
	}
	else if (b == SymbolType::String) {
		if (a.name == "str") {
			dif = 1;
			return true;
		}
		else
			dif = 200;
	}
	else if (b == SymbolType::Variable) {
		auto val = std::get<std::string>(v);
		auto type = FindTypeForVariable(vm, val);
		//Animal == Duck
		return CheckIfTypeFits(vm, a, type);
	}
	else {
		printf("Not implemented: 105\n");
	}
	dif = abs(dif);
	if (dif > 100)
		return false;
	else
		return true;
}
Status Token::FindQualified(VM& vm, std::vector<DemangledStruct>& demangled, SymbolType type, Mixed v, uint8_t index, const std::string& name) {
	std::string best_qualified = "";
	demangled.erase(std::remove_if(demangled.begin(), demangled.end(), [&](DemangledStruct& d) {
		if (demangled.size() == 0)
			return true;
		if (d.args.size() == 0)
			return true;
		if (d.args.back()->basic == BasicType::variadic && index >= d.args.size() - 1)
			return false;
		if (demangled.size() == 1)
			best_qualified = d.mangled;
		if (d.args.size() <= index)
			return true;
		//Remove any not of same type.
		if (!SameType(vm, *d.args[index], type, v, d.diff, d.needs_compilation))
			//if (local_class && index == 0) {
				//if (!SameType(vm, *d.args[index], type, local_class->name, d.diff, d.needs_compilation))
					//return true;
			//}
			//else
				return true;
		
		return false;
	}), demangled.end());

	if (demangled.size() == 0) {
		//printf("No matching function for %s.\n", name.data());
		//printf("Nearest match: %s.\n", best_qualified.data());
		return EXIT;
	}
	std::sort(demangled.begin(), demangled.end(), [](const auto& lhs, const auto& rhs) {
		return lhs.diff < rhs.diff;
	});
	return CONTINUE;
}
std::pair<std::string, std::string> Helper::ModifyMangled(const std::string& in, const DemangledStruct& demangled) {
	auto split = Helper::split_1(in, '_');
	std::string out = "";
	std::string types = "";
	//if (split[0] == "$")
		out += "_" + demangled.type.type_name;
	out += "_" + demangled.name;
	for (int i = 2; i < split.size(); ++i) {
		if (split[i] == "$") {
			split[i] = demangled.args[i-2]->name;
		}
		out += "_" + split[i];
		if (BasicTypeCheck(demangled.args[i - 2]->name))
			types += demangled.args[i - 2]->name + ",";
		else
			types += demangled.args[i - 2]->name + "*,";
	}
	if (types.size() > 0)
		types.pop_back();
	else
		out += "_";
	return { out, types };
}
std::string Helper::Mangle(DemangledStruct& demangled) {
	std::string mangled = "";
	if (demangled.names.size() == 0) {
		mangled = "entry1:\n " + demangled.name + "(";
		for (int i = 0; i < demangled.args.size(); ++i) {
			mangled += demangled.args[i]->name + ",";
		}
		if (demangled.args.size() > 0)
			mangled.pop_back();
		mangled += "):" + demangled.type.type_name;
		auto split = Helper::split_1(demangled.body, '=');
		if (split.size() > 0)
			mangled += "=" + split[1];
		//else
			//printf("Error?\n");
		demangled.is_pointer = true;
		//string, u8):void = clib.get(\"printf\")";// demangled.body;
	}
	else {
		mangled = demangled.name + "(";
		for (int i = 0; i < demangled.args.size(); ++i) {
			mangled += demangled.names.at(i) + ":" + demangled.args[i]->name + ",";
		}
		if (demangled.args.size() > 0)
			mangled.pop_back();
		mangled += "):";
		//mangled += "):" + demangled.type + ":";
		mangled += demangled.body;
	}
	return mangled;
}

bool Token::FindMostQualified(std::vector<std::pair<SymbolType, Mixed>>& symbols, uint32_t index, std::string& name, DemangledStruct& out, Status& restart, bool can_restart) {
	auto first = symbols.front();
	if (symbols[index].first == SymbolType::Function)
		first = symbols[index];
	else
		for (auto& f : symbols) {
			if (f.first == SymbolType::Function || f.first == SymbolType::Equals) {
				first = f;
				break;
			}

		}
	auto& vm = Parsed::get()->getVM();
	auto name2 = std::get<std::string>(first.second);
	int found_period = name2.find('.');
	std::string method_sig = "";
	if (found_period > -1) {
		//get mangled function name
		auto split = Helper::split_1(name2, '.');
		auto var_name = split[0];
		auto var_method = split[1];
		//find the variable in the vm and return its type, so we can find the method
		auto token = vm.labels.find(var_name);
		if (token != vm.labels.end()) {
			auto var_type = token->second.GetType();
			name2 = var_method + "_" + var_type.type_name;
		}
		else {
		}

	}
	ClassToken* local_class = nullptr;
	//if has $ then is a templated function, so find root function and create new function if it does not exist already

	if (CreateClass(vm, name2) == RESTART) {
		restart = RESTART;
		return false;
	}

	auto possible_functions = vm.functions.find(name2);
	auto possible_functions2 = vm.template_functions.find(name2);
	auto possible_functions4 = vm.labels.find(name2);
	auto _parent = GetClassOrFunction(parent);
	if (_parent->token_type == THE_TOKEN_TYPE::CLASS) {
		local_class = (ClassToken*)_parent;
	}
	std::vector<DemangledStruct> demangled;
	if (local_class) {
		std::string name1 = name + "_" + local_class->name;
		auto possible_functions3 = vm.functions.find(name1);
		if (possible_functions3 != vm.functions.end())
			for (auto d : possible_functions3->second)
				demangled.push_back(Helper::Demangle(d->name, {}, ""));
	}
	if (possible_functions != vm.functions.end())
		for (auto d : possible_functions->second)
			demangled.push_back(Helper::Demangle(d->name, {}, ""));

	if (possible_functions4 != vm.labels.end())
		demangled.push_back(Helper::Demangle(possible_functions4->first, {}, ""));
	if (demangled.size() == 0 /*|| ((int)name2.find("_$") < 0 && possible_functions2 != vm.template_functions.end())*/) {
		//printf("No function (%s).\n", name2.data());
		return false;
	}
	if (possible_functions2 != vm.template_functions.end())
		for (auto d : possible_functions2->second)
			demangled.push_back(Helper::Demangle(d.first, d.second.args, d.second.body));
	int arg_n = 0;
	int arg_n2 = 0;
	bool has_possible = true;
	SymbolType t0 = SymbolType::NA, t1= SymbolType::NA;
	for (uint16_t i = index; i < symbols.size() - 1; ++i) {
		t1 = t0;
		auto v = symbols[i + 1];
		t0 = v.first;
		//this is designed to check if is a seperate variable or just a mod to a variable ie.(-,+,*,/).
		if (t1 != SymbolType::ParenLeft && t1 != SymbolType::NA && t1 != SymbolType::Comma)
			continue;
		if (v.first == SymbolType::Equals)
			break;
		if (v.first == SymbolType::Type)
			arg_n2++;

		if (v.first != SymbolType::Variable && v.first != SymbolType::U8 && v.first != SymbolType::U16 && v.first != SymbolType::U32 && v.first != SymbolType::U64 && v.first != SymbolType::I8 && v.first != SymbolType::I16 && v.first != SymbolType::I32 && v.first != SymbolType::I64 && v.first != SymbolType::Double && v.first != SymbolType::Float && v.first != SymbolType::String && v.first != SymbolType::Char)
			continue;
		//has_possible = true;
		//Find all qualified functions, put most qualified at front
		if (FindQualified(vm, demangled, v.first, v.second, arg_n, name) == EXIT) {
			restart = EXIT;
			return false;
		}
		arg_n++;
		if (demangled.size() == 0) {
			return false;
		}
	}
	if (arg_n2 != demangled.front().args.size() && arg_n == 0)
		return false;
	auto got = vm.labels.find(demangled.front().mangled + "$");
	if (got != vm.labels.end() && demangled.front().mangled != name) {
		demangled.front().mangled += "$";
	//	printf("Found label\n");
	}
	if (demangled.front().needs_compilation) {
		auto test = Helper::Mangle(demangled.front());
		if (demangled.front().is_pointer) {
			auto& libName1 = vm.labels[demangled.front().mangled];
			auto s = Helper::ModifyMangled(demangled.front().mangled, demangled.front());
			auto& libName = vm.create_ui64(s.first, libName1.ptr);
			libName.position = 8; //set as having 8 bytes
			std::string complex_type = "";
			//type = ReplaceUnsafe(type);
			auto type = demangled.front().type;
			libName.SetType(type);
			libName.basic = type.type;
			if (libName.def.size() == 0) {
				auto t = type.GetComplexType();
				libName.def = t + "(*" + s.first + ")(" + s.second + ") = (void*)0x0;" + s.first + ";" + std::to_string(libName1.ptr) + "\n"; //possibly use libName.ptr
			}
			//libName.def = "extern " + libName.GetComplexType() + " " + demangled.front().mangled + ";" + demangled.front().mangled + ";" + std::to_string(libName1.ptr) + "\n";
			vm.functions[name2].push_back(&libName);
		}
		else {
			if (!can_restart)
				return false;
			//auto old = Parsed::get()->root->children["entry1"]->needs_compile;
			//Parsed::get()->root->children["entry1"]->needs_compile = false;
			auto ret = Parser::getMachineCode(vm, test);
			restart = RESTART;
			//Parsed::get()->root->children["entry1"]->needs_compile = old;
			if (ret) {
				demangled.front().needs_compilation = false;
			}
		}
	}
	else
		restart = CONTINUE;
	if (!has_possible)
		return false;
	out = demangled.front();
	for (auto a : out.args)
		AddClass(vm, a->name);
	return true;
}
std::string ReplaceUnsafe(const std::string& str) {
	std::string out = str;
	Helper::replaceAll(out, " ", "$");
	Helper::replaceAll(out, "*", ""); //$ptr
	return out;
}
Status FunctionToken::PreProcess(VM& vm) {
	if (!needs_compile) return CONTINUE;
	if (name.back() == '$') {
		name.pop_back();
		is_virtual = true;
	}
	Status restart = CONTINUE;
	std::string name1 = "";
	std::string type1 = "";
	std::pair<std::string, std::string> type2 = { "", "" };
	std::string mangled_types = "";
	//Helper::replaceAll(name, "_", "$");
	if (name.size() == 0) {
		printf("Error: No name for function.\n");
		return restart;
	}
	if (parent) {
		if (!global and parent->name.size() != 0) {
			//type = { BasicType::var, parent->name };
			//args["T$"] = StringToType(parent->name);
			//mangled_types = parent->name + "_";
			//types = parent->name + "*,";
			if (parent->name != name) {
				symbols.insert(symbols.begin(), { SymbolType::Variable, "T$" });
				symbols.insert(symbols.begin() + 1, { SymbolType::Colon, SymbolType::Colon });
				symbols.insert(symbols.begin() + 2, { SymbolType::Type, parent->name });
			}
			//name = parent->name + "$" + name;
		}
	}
	bool expecting_ret = false;
	for (uint16_t i = 0; i < symbols.size(); ++i) {
		auto v = symbols[i];
		if (v.first == SymbolType::Variable) {
			//check if last variable had its type set, if not then find its type
			if (name1.size() > 0) {
				args[name1] = { BasicType::var, type1 };
			}
			name1 = std::get<std::string>(v.second);
		}
		else if (v.first == SymbolType::Type) {
			type2.first = type2.second;
			type1 = std::get<std::string>(v.second);
			type2.second = type1;
			//if (type2.first.size() != 0)
				//type1 = type2.first;
			if (auto token = vm.FindClass(type1)) {
				if (type2.first.size() != 0)
					AddClass(vm, type1, { type2.first });
				else
					AddClass(vm, type1, { type1 });
				if (symbols.size() > i + 1 && i > 0) {
					auto last = symbols[i - 1];
					auto next = symbols[i + 1];
					if (last.first == SymbolType::Const)
						type1 = "const " + type1;
					//if (next.first == SymbolType::Star)
						//type1 = type1 + "*";
					//if (!BasicTypeCheck(type1))
						//type1 = type1 + "*";
					//#define v *(v)
				}
			}
			else
				printf("Unable to find type:%s ERRCODE:302\n", type1.data());
			if (!expecting_ret) {
				if (Helper::BasicTypeCheck(type1))
					types += type1 + ",";
				else
					types += type1 + "*,";
				mangled_types += ReplaceUnsafe(type1) + "_";
				if (name1.size() > 0) {
					args[name1] = { StringToBasic(type1), type1 };
					AddVariable(vm, name1);
					AddClass(vm, type1);
					//args[name1] = { name1, type1 };
					//already_set.emplace(name1);
					name1 = "";
					type1 = "";
				}
			}
			else
				this->type = StringToType(type1);
			
		}
		else if (v.first == SymbolType::ArgsEnd) {
			expecting_ret = true;
			if (name1.size() > 0) {
				args[name1] = { StringToBasic(type1), type1 };
			}
		}
		else if (v.first == SymbolType::Equals) {
			importing_func = true;
		}
	}
	if (types.size() > 0)
		types.pop_back();
	if (mangled_types.size() > 0)
		mangled_types.pop_back();
	for (auto arg : args) {
		if (arg.second.type == BasicType::nul) {
			//Can't build this function yet, will be built once first called so we can get the type.
			mangled_name = MangleFunction(vm);
			std::vector<std::string> args2;
			for (auto arg1 : args)
				args2.push_back(arg1.first);
			vm.addTemplateFunction(name, mangled_name, _body, args2);
			is_template = true;
			//return;
		}
	}
	restart_children:
	for (auto& c : children) {
		auto status = c.second->PreProcess(vm);

		if (status == RESTART)
			return RESTART;
		else if (status == EXIT)
			return EXIT;
		if (c.second->name.size() != 0) {
			std::string n = c.second->name;
			if (auto got = vm.FindClass(parent->name)) {
				if (got->variables.find(n) != got->variables.end()) {
					n = "T$->" + n;
					c.second->name = n;
					c.second->mangled_name = n;
				}
			}
			if (!c.second->global) {
				auto got2 = locals.find(n);
				if (got2 != locals.end()) {
					if (locals[n].type == BasicType::nul)
						locals[n] = c.second->type;
				}
				else
					locals[n] = c.second->type;
			}
		}
	}
	if (type.type == BasicType::nul && children.size() > 0) {
		auto c = children.back();
		if (c.second->name == "" && c.second->type.type == BasicType::nul) {
			if (c.second->symbols.size() > 0) {
				if (c.second->symbols[0].first == SymbolType::Function) {
					//is function, get functions return type
					auto n = std::get<std::string>(c.second->symbols[0].second);
					DemangledStruct out_struct;
					if (!FindMostQualified(c.second->symbols, 0, n, out_struct, restart)) {
						return restart;
					}
					if (restart)
						return restart;
					auto got = vm.labels.find(out_struct.mangled);
					if (got != vm.labels.end()) {
						c.second->type = got->second.GetType();
						//if (!BasicTypeCheck(c.second->type))
							//c.second->type += '*';
					}
					else {
						c.second->type = StringToType("u8");
						printf("Super can't find return type for %s ERRCODE:731\n", name.data());
						//return;
					}
				}
				else if (c.second->symbols[0].first >= SymbolType::I8 && c.second->symbols[0].first <= SymbolType::Double) {
					c.second->type = SymbolToType(c.second->symbols[0].first);
				}
				else if (c.second->symbols[0].first == SymbolType::False || c.second->symbols[0].first == SymbolType::True) {
					c.second->type = StringToType("bool");
				}
				else if (c.second->symbols[0].first == SymbolType::Variable) {
					//c.second->type = "bool";
				}
				else {
					c.second->type = SymbolToType(c.second->symbols[0].first);
					//printf("Kan't find return type for %s\n", name);
				}
			}
			else {
				printf("Can't find return type for %s\n", name.data());
			}
		}
		else if (c.second->type.type == BasicType::nul) {
			auto got = args.find(c.second->name);
			if (got != args.end())
				c.second->type = got->second;
			else {
				auto got2 = locals.find(c.second->name);
				if (got2 != locals.end())
					c.second->type = got2->second;
			}
		}
		type = c.second->type;
		if (type.type != BasicType::nul) {
			AddClass(vm, type.type_name);
		}
	}
	if (name == "entry1")
		mangled_name = "__" + name + "_" + mangled_types;
	else
		mangled_name = "_" + type.type_name + "_" + name + "_" + mangled_types;
	if (is_virtual)
		mangled_name += '$';
	
	auto& label = vm.label(mangled_name, std::vector<uint8_t>{ 0 });
	label.SetType(type);
	vm.functions[name].push_back(&label);
	label.by = By::val;
	label.basic = type.type;
	auto nn = label.GetType();
	if (!importing_func)
		label.def = "extern " + nn.GetComplexType() + " " + mangled_name + "(" + types + ");" + mangled_name + ";" + std::to_string(label.ptr) + "\n";
	else
		label.def = nn.GetComplexType() + "(*" + mangled_name + ")(" + types + ") = (void*)0x0;" + mangled_name + ";" + std::to_string(label.ptr) + "\n";
	//auto& label = vm.label(mangled_name, {});
	//label.SetType(type);
	//vm.functions[name].push_back(&label);
	return restart;
}
int globals_count = 0;
bool IsType(const SymbolType& type) {
	if (type >= SymbolType::Type && type <= SymbolType::Double)
		return true;
	return false;
}
std::string Type::GetType() {
	return type_name;
}
std::string Type::GetComplexType() {
	std::string temp = "";
	if (isArray)
		temp = "$";
	if (Helper::BasicTypeCheck(*this))
		return type_name + temp;
	else
		return type_name + temp + "*";
}
Type Token::mixedToType2(VM& vm, std::pair<SymbolType, Mixed>& v) {
	if (v.first == SymbolType::U8)
		return { BasicType::u8, "u8", std::to_string(std::get<uint64_t>(v.second)) };
	else if (v.first == SymbolType::U16)
		return { BasicType::u16, "u16", std::to_string(std::get<uint64_t>(v.second)) };
	else if (v.first == SymbolType::U32)
		return { BasicType::u32, "u32", std::to_string(std::get<uint64_t>(v.second)) };
	else if (v.first == SymbolType::U64)
		return { BasicType::u64, "u64", std::to_string(std::get<uint64_t>(v.second)) };
	else if (v.first == SymbolType::I8 || v.first == SymbolType::Char)
		return { BasicType::i8, "i8", std::to_string(std::get<int64_t>(v.second)) };
	else if (v.first == SymbolType::I16)
		return { BasicType::i16, "i16", std::to_string(std::get<uint64_t>(v.second)) };
	else if (v.first == SymbolType::I64)
		return { BasicType::i32, "i32", std::to_string(std::get<uint64_t>(v.second)) };
	else if (v.first == SymbolType::I64)
		return { BasicType::i64, "i64", std::to_string(std::get<uint64_t>(v.second)) };
	else if (v.first == SymbolType::String)
		return { BasicType::str, "str", std::get<std::string>(v.second) };
	else if (v.first == SymbolType::Variable) {
		auto str = std::get<std::string>(v.second);
		auto type = FindTypeForVariable(vm, str);
		v.second = str;
		type.value = v.second;
		return type;
	}
	else if (v.first == SymbolType::Type) {
		auto str = std::get<std::string>(v.second);
		auto type = FindTypeForVariable(vm, str);
		v.second = str;
		//type.value = v.second;
		return type;
	}
}
Status VariableToken::PreProcess(VM& vm) {
	Status restart = CONTINUE;
	if (!needs_preprocess) return restart;
	bool importing = false;
	for (auto& a : symbols) {
		if (a.first == SymbolType::TypeOf) {
			auto str = std::get<std::string>(a.second);
			auto t = FindTypeForVariable(vm, str);
			a.first = SymbolType::String;
			a.second = t.type_name;

		}
	}
	for (uint16_t i = 0; i < symbols.size(); ++i) {
		auto& a = symbols[i];
		if (a.first == SymbolType::ArrayStart) {
			std::vector<Type> types;
			bool end_braket_found = false;
			int ii = 0;
			Type _type;
			int type_count = 0;
			int arg_count = 0;
			int array_start = i;
			for (int n = i; n < symbols.size(); ++n) {
				if (IsType(symbols[n].first)) {
					auto temp_type = mixedToType2(vm, symbols[n]);
					if (_type.type != temp_type.type) {
						_type = temp_type;
						type_count++;
					}
					type = { BasicType::var, "vec$" + _type.GetType() };
				}
				else if (symbols[n].first == SymbolType::ArrayEnd) {
					end_braket_found = true;
					i += n - i;
					break;
				}
				else {
					arg_count++;
				}

			}
			if (arg_count == 1) break;
			int array_end = i + 1;
			auto _t = "_" + type.type_name + "_" + type.type_name + "_";
			auto _r = "_void_resize_" + type.type_name + "_u8";
			std::vector<std::pair<SymbolType, Mixed>> _syms2;
			_syms2.insert(_syms2.begin(), symbols.begin() + array_start, symbols.begin() + array_end);
			auto _temp = new VariableToken(_syms2);
			_temp->name = name;
			_temp->parent = this;
			_temp->needs_preprocess = false;
			symbols[array_start] = { SymbolType::Function, _t };
			symbols[array_start + 1] = { SymbolType::ArgsStart, "(" };
			symbols[array_start + 2] = { SymbolType::ArgsEnd, ")" };
			std::vector<std::pair<SymbolType, Mixed>> syms = { {SymbolType::Function, _r}, { SymbolType::ArgsStart, "(" }, { SymbolType::Variable, name }, { SymbolType::Comma, "," }, { SymbolType::U8, (uint64_t)arg_count }, {SymbolType::ArgsEnd, ")"} };
			children["a|"] = new VariableToken(syms);
			children["a|"]->parent = this;
			children["a||"] = _temp;
			AddVariable(vm, _r);
			for (auto _i = array_start + 3; _i < array_end; ++_i)
				symbols[_i] = { SymbolType::NA, "" };
			if (IsType(symbols[i - 1].first)) {
				type.isArray = true;
				type.type = BasicType::var;
			}
			if (!end_braket_found)
				printf("Error: No matching bracket for array.\n");
			if (CreateClass(vm, type.type_name) == RESTART) {
				auto& libName = vm.label(name, (uint64_t)0);
				libName.def = "extern vec$u8* " + name + ";" + name + ";" + std::to_string(libName.ptr) + "\n";
				libName.SetType(type);
				return RESTART;
			}
		}
	}
	if (symbols.size() > 0)
		if (symbols[0].first == SymbolType::Function && name == "") {

			importing = true;
			int ii = 0;

			//for (auto s : symbols) {
				//if (s.first == SymbolType::Equals) {
					//we are assigning a function pointer to a variable
					auto n = std::get<std::string>(symbols[0].second);
					DemangledStruct out_struct;
					auto split = Helper::split_1(n, '.');
					if (split.size() > 1) {
						auto var_name = split[0];
						auto var_method = split[1];
						auto token = vm.labels.find(var_name);
						if (token != vm.labels.end()) {
							auto var_type = token->second.GetType();
							auto t = FindTypeForVariable(vm, var_name);
							symbols[0].second = var_method;// +"_" + t.GetType();
							symbols.insert(symbols.begin() + 1, { SymbolType::Variable, var_name });
							if (symbols[2].first != SymbolType::ArgsEnd)
								symbols.insert(symbols.begin() + 2, { SymbolType::Comma, "," });
							n = var_name;
						}
						else {
							auto token = parent->children.find(var_name);
							if (token != parent->children.end()) {
								//get type from block
							}
						}
					}
					Status restart2 = CONTINUE;
					auto result = FindMostQualified(symbols, 0, n, out_struct, restart2, true);

					//if (RESTART == restart)
						//return restart;
					if (!result) {
						symbols.erase(symbols.begin());
						name = n;
						//create function definition
						std::string types = "";
						std::string mangled_types = "";
						bool expecting_ret = false;
						bool is_variadic = false;
						std::string temp_type = "";
						for (uint16_t i = 0; i < symbols.size(); ++i, ++ii) {
							auto v = symbols[i];
							if (IsType(v.first)) {
								//type = FindTypeForVariable(vm, (FunctionToken*)parent, std::get<std::string>(v.second));
								type = SymbolToType(v.first);
								if (type.type == BasicType::var) {
									auto temp = std::get<std::string>(v.second);
									type = FindTypeForVariable(vm, temp);
								}
							}
							else if (v.first == SymbolType::ArgsEnd) {
								expecting_ret = true;
								if (v == symbols.back() && type.type == BasicType::nul && this->type.type == BasicType::nul)
									this->type = { BasicType::nul, "void" };
								continue;
							}
							else {
								continue;
							}
							if (type.type == BasicType::nul)
								type = {BasicType::template_type, "_"};
							AddClass(vm, type.type_name);
							std::string s = "";
							if (!expecting_ret) {
								if (type.type == BasicType::template_type || type.type == BasicType::variadic) {
									temp_type = "#";
									is_variadic = true;
								}
								if (Helper::BasicTypeCheck(type))
									types += s + type.type_name + ",";
								else
									types += s + type.type_name + "*,";
								mangled_types += type.type_name + "_";
							}
							else {
								if (type.type == BasicType::nul)
									type = { BasicType::template_type, "_" };
								this->type = type;
								ii++;
								break;
							}
						}

						if (types.size() > 0)
							types.pop_back();
						if (mangled_types.size() > 0)
							mangled_types.pop_back();
						mangled_name = "_" + type.type_name + "_" + name + "_" + mangled_types;

						//Helper::replaceAll(mangled_name, "#", "$");
						Helper::replaceAll(types, "#", "$");
						Helper::replaceAll(mangled_name, "#", "$");
						auto& label = vm.label(mangled_name, (uint64_t)0); //creates emptry function pointer, will assign at runtime
						//vm.functions[name].push_back(&label);
						//type = ReplaceUnsafe(type);
						label.SetType(type);
						label.basic = type.type;
						auto t = label.GetType().type_name;
						label.def = t + "(*" + mangled_name + ")(" + types + ") = (void*)0x0;" + mangled_name + ";" + std::to_string(label.ptr) + "\n";
						if (is_variadic) {
							vm.addTemplateFunction(name, mangled_name, parent->_body, {});
							label.def = t + "(*" + mangled_name + ")(" + types + ") = (void*)0x0;" + mangled_name + ";" + std::to_string(label.ptr) + "\n";
							//return CONTINUE;
							//needs_compile = false;
							//return CONTINUE;
						}
						else
							vm.functions[name].push_back(&label);
						//break;
					//}
						if (ii > 0)
							symbols.erase(symbols.begin(), symbols.begin() + ii);
					}
					else {
						type = out_struct.type;
						AddVariable(vm, out_struct.mangled);
						for (auto a : out_struct.args)
							AddClass(vm, a->name);
					}
		}
		else if (symbols.size() > 1) {
			if (symbols[1].first == SymbolType::Function) {
				std::string n = std::get<std::string>(symbols[1].second);
				DemangledStruct out_struct;
				auto result = FindMostQualified(symbols, 1, n, out_struct, restart);
				if (restart == RESTART || restart == EXIT)
					return restart;
				auto got = vm.labels.find(out_struct.mangled);
				if (got != vm.labels.end()) {
					if (type.type == BasicType::nul) {
						Token* func_parent = parent;
						while (func_parent) {
							if (func_parent->token_type == THE_TOKEN_TYPE::FUNCTION) {
								AddVariable(vm, out_struct.mangled);
								AddClass(vm, got->second.GetType().type_name);
								break;
							}
							func_parent = func_parent->parent;
						}
						type = got->second.GetType();
					}
				}
			}
		}
	bool is_const = false;
	if (mangled_name.size() == 0)
		mangled_name = name;
	bool setting_value = false;
	for (uint16_t i = 0; i < symbols.size(); ++i) {
		auto& v = symbols[i];
		if (v.first == SymbolType::Equals) {
			setting_value = true;
		}
		else if (v.first == SymbolType::Star) {
			type.isNativeArray = true;
		}
		else if (v.first == SymbolType::Float) {
			/*if (name.size() > 0) {
				symbols[i].first = SymbolType::Variable;
				auto temp = std::get<double>(v.second);
				auto& libName = vm.label(mangled_name, (float)temp);
				libName.def = "extern f32 " + mangled_name + ";" + mangled_name + ";" + std::to_string(libName.ptr) + "\n";
				libName.SetType(StringToType("f32"));
				symbols[i].second = mangled_name;
			}
			else {*/
				symbols[i].first = SymbolType::Variable;
				auto n = "_S" + std::to_string(globals_count++);
				auto temp = std::get<double>(v.second);
				/*if (fabs(temp) > UINT32_MAX) {
					auto& libName = vm.label(name, temp);
					libName.def = "extern f64 " + name + ";" + name + ";" + std::to_string(libName.ptr) + "\n";
					libName.SetType("f64");
				}*/
				auto& libName = vm.label(n, (float)temp);
				libName.def = "extern f32 " + n + ";" + n + ";" + std::to_string(libName.ptr) + "\n";
				libName.SetType(StringToType("f32"));
				symbols[i].second = n;
			//}
			continue;
		}
		else if (v.first == SymbolType::String) {
			auto d = std::get<std::string>(v.second);
			symbols[i].first = SymbolType::Variable;
			std::string name2 = name;// +"_S";
			//name = parent->name + name;// +"_S";
			//mangled_name = name;
			if (name.size() > 0) {
				auto b = symbols[i - 1];
				if (b.first == SymbolType::Function) {
					std::string n = "_S" + std::to_string(globals_count++);
					name2 = n;

				}
				if (b.first == SymbolType::Comma) {
					std::string n = "_S" + std::to_string(globals_count++);
					name2 = n + "1";
				}
				if (global && parent->name == "entry1") {
					if (type.type_name.size() == 0)
						type = StringToType("str");
					auto& libName0 = vm.label(name2, d);
					libName0.def = "extern str* " + name2 + ";" + name2 + ";" + std::to_string(libName0.ptr) + "\n";
					libName0.SetType(StringToType("str"));
					symbols[i].second = name2;
				}
				else {
					if (type.type_name.size() == 0)
						type = StringToType("str");
					std::string n = "_S" + std::to_string(globals_count++);
					auto& libName0 = vm.label(n, d);
					libName0.def = "extern str* " + n + ";" + n + ";" + std::to_string(libName0.ptr) + "\n";
					libName0.SetType(StringToType("str"));
					symbols[i].second = n;
					//auto& libName1 = vm.label(name, libName0.ptr);
					//libName1.SetType(StringToType("str"));
					//libName1.def = "extern str* " + name + ";" + name + ";" + std::to_string(libName1.ptr) + "\n";

				}
			}
			else {
				std::string n = "_S" + std::to_string(globals_count++);
				auto& libName0 = vm.label(n, d);
				libName0.def = "extern str* " + n + ";" + n + ";" + std::to_string(libName0.ptr) + "\n";
				libName0.SetType(StringToType("str"));
				symbols[i].second = n;
			}
			continue;
		}
		else if (v.first == SymbolType::Variable) {
			auto n = std::get<std::string>(v.second);
			int found_period = n.find('.');
			std::string method_sig = "";
			if (found_period > -1) {
				//get mangled function name
				auto split = Helper::split_1(n, '.');
				auto var_name = split[0];
				auto var_method = split[1];
				//find the variable in the vm and return its type, so we can find the method
				auto token = vm.labels.find(var_name);
				if (token != vm.labels.end()) {
					auto var_type = token->second.GetType();
					method_sig = var_method + "_" + var_type.type_name;
					n = var_name;
				}
				else {
				}
			}
			else {
			}
		}
		if (v.first == SymbolType::Function) {
			auto n = std::get<std::string>(v.second);
			int found_period = n.find('.');
			std::string method_sig = "";
			if (found_period > -1) {
				auto split = Helper::split_1(n, '.');
				auto var_name = split[0];
				auto var_method = split[1];
				auto token = vm.labels.find(var_name);
				if (token != vm.labels.end()) {
					auto var_type = token->second.GetType();
					method_sig = var_method + "_" + var_type.type_name;
					v.second = method_sig;
					symbols.insert(symbols.begin() + i + 1, { SymbolType::Variable, var_name });
					if (symbols[i + 2].first != SymbolType::ArgsEnd)
						symbols.insert(symbols.begin() + i + 2, { SymbolType::Comma, "," });
					n = var_name;
				}
				else {
					auto token = parent->children.find(var_name);
					if (token != parent->children.end()) {
						//get type from block
					}
				}
			}
		}
 		if (setting_value) {
			if (symbols[0].first == SymbolType::Member) continue;
			if (v.first == SymbolType::U64) {
				type = { BasicType::u64, "u64" };
				if (global && name != "") {
					auto temp = std::get<std::uint64_t>(v.second);
					auto& libName = vm.label(mangled_name, (uint64_t)temp);
					libName.def = "extern u64 " + mangled_name + ";" + mangled_name + ";" + std::to_string(libName.ptr) + "\n";
					libName.SetType(type);
				}
			}
			else if (v.first == SymbolType::SizeOf) {
				type = { BasicType::u64, "u64" };
				if (global && name != "") {
					auto& libName = vm.label(mangled_name, (uint64_t)0);
					libName.def = "extern u64 " + mangled_name + ";" + mangled_name + ";" + std::to_string(libName.ptr) + "\n";
					libName.SetType(type);
				}
			}
			else if (v.first == SymbolType::U32) {
				if (global && name != "") {
					auto temp = std::get<std::uint64_t>(v.second);
					auto& libName = vm.label(mangled_name, (uint32_t)temp);
					libName.def = "extern u32 " + mangled_name + ";" + mangled_name + ";" + std::to_string(libName.ptr) + "\n";
					libName.SetType(SymbolToType(v.first));
				}
			}
			else if (v.first == SymbolType::U16) {
				if (global && name != "") {
					auto temp = std::get<std::uint64_t>(v.second);
					auto& libName = vm.label(mangled_name, (uint16_t)temp);
					libName.def = "extern u16 " + mangled_name + ";" + mangled_name + ";" + std::to_string(libName.ptr) + "\n";
					libName.SetType(SymbolToType(v.first));
				}
			}
			else if (v.first == SymbolType::U8) {
				if (global && name != "") {
				if (auto got = vm.labels.find(mangled_name) == vm.labels.end()) {
					type = mixedToType2(vm, v);
					auto temp = std::get<std::uint64_t>(v.second);
					auto& libName = vm.label(mangled_name, (uint8_t)temp);
					libName.def = "extern u8 " + mangled_name + ";" + mangled_name + ";" + std::to_string(libName.ptr) + "\n";
					libName.SetType(SymbolToType(v.first));
				}
				}
			}
			else if (v.first == SymbolType::I64) {
				if (global && name != "") {
					auto temp = std::get<std::int64_t>(v.second);
					auto& libName = vm.label(mangled_name, (int64_t)temp);
					libName.def = "extern i64 " + mangled_name + ";" + mangled_name + ";" + std::to_string(libName.ptr) + "\n";
					libName.SetType(SymbolToType(v.first));
				}
			}
			else if (v.first == SymbolType::I32) {
				if (global && name != "") {
					auto temp = std::get<std::int64_t>(v.second);
					auto& libName = vm.label(mangled_name, (int32_t)temp);
					libName.def = "extern i32 " + mangled_name + ";" + mangled_name + ";" + std::to_string(libName.ptr) + "\n";
					libName.SetType(SymbolToType(v.first));
				}
			}
			else if (v.first == SymbolType::I16) {
				if (global  && name != "") {
					auto temp = std::get<std::int64_t>(v.second);
					auto& libName = vm.label(mangled_name, (int16_t)temp);
					libName.def = "extern i16 " + mangled_name + ";" + mangled_name + ";" + std::to_string(libName.ptr) + "\n";
					libName.SetType(SymbolToType(v.first));
				}
			}
			else if (v.first == SymbolType::I8) {
				if (global && name != "") {
					auto temp = std::get<std::int64_t>(v.second);
					auto& libName = vm.label(mangled_name, (int8_t)temp);
					libName.def = "extern i8 " + mangled_name + ";" + mangled_name + ";" + std::to_string(libName.ptr) + "\n";
					libName.SetType(SymbolToType(v.first));
				}
			}
			else if (v.first == SymbolType::Float) {
				if (global && name != "") {
					symbols[i].first = SymbolType::Variable;
					auto temp = std::get<double>(v.second);
					auto& libName = vm.label(mangled_name, (float)temp);
					libName.def = "extern f32 " + mangled_name + ";" + mangled_name + ";" + std::to_string(libName.ptr) + "\n";
					libName.SetType(SymbolToType(v.first));
					symbols[i].second = mangled_name;
				}
			}
			else if (v.first == SymbolType::Double) {
				if (global && name != "") {
					symbols[i].first = SymbolType::Variable;
					auto temp = std::get<double>(v.second);
					auto& libName = vm.label(mangled_name, temp);
					libName.def = "extern f64 " + mangled_name + ";" + mangled_name + ";" + std::to_string(libName.ptr) + "\n";
					libName.SetType(SymbolToType(v.first));
					symbols[i].second = mangled_name;
				}
			}
			else if (v.first == SymbolType::Variable) {
				if (global && name != "") {
					//reserve ui64 for pointer
					if (mangled_name == "")
						mangled_name = name;
					auto& libName = vm.create_ui64(mangled_name, 1);
					libName.position = 8; //set as having 8 bytes
					std::string complex_type = "";
					if (!importing) {
						//get a list of eligible mangled functions
					}
					//type = ReplaceUnsafe(type);
					libName.SetType(type);
					libName.basic = type.type;
					auto n = libName.GetType();
					if (libName.def.size() == 0)
						libName.def = "extern " + n.GetComplexType() + " " + mangled_name + ";" + mangled_name + ";" + std::to_string(libName.ptr) + "\n";
				}
			}
			else if (v.first == SymbolType::Function) {
				auto n = std::get<std::string>(v.second);
				int found_period = n.find('.');
				std::string method_sig = "";
				if (found_period > -1) {
					//get mangled function name
					auto split = Helper::split_1(n, '.');
					auto var_name = split[0];
					auto var_method = split[1];
					//find the variable in the vm and return its type, so we can find the method
					auto token = vm.labels.find(var_name);
					if (token != vm.labels.end()) {
						auto var_type = token->second.GetType();
						method_sig = var_method + "_" + var_type.type_name;
						v.second = method_sig;
						symbols.insert(symbols.begin() + i + 1, { SymbolType::Variable, var_name });
						if (symbols[i + 2].first != SymbolType::ArgsEnd)
							symbols.insert(symbols.begin() + i + 2, { SymbolType::Comma, "," });
						n = var_name;
					}
					else {
						auto token = parent->children.find(var_name);
						if (token != parent->children.end()) {
							//get type from block

						}
					}
				}
				if (global && name != "") {
					//reserve ui64 for pointer
					if (mangled_name == "")
						mangled_name = name;
					auto& libName = vm.create_ui64(mangled_name, 1);
					libName.position = 8; //set as having 8 bytes
					std::string complex_type = "";
					if (!importing) {
						if (method_sig.size() == 0)
							method_sig = n;
					}
					//type = ReplaceUnsafe(type);
					libName.SetType(type);
					libName.basic = type.type;
					auto n = libName.GetType();
					if (libName.def.size() == 0)
						libName.def = "extern " + n.GetComplexType() + " " + mangled_name + ";" + mangled_name + ";" + std::to_string(libName.ptr) + "\n";
				}
				else {

				}
				setting_value = false;
			}
		}
	}
	if (symbols.size() > 0 && type.type == BasicType::nul) {
		if (symbols[0].first == SymbolType::Member) {
			std::string string = name + "." + std::get<std::string>(symbols[0].second);
			type = FindTypeForVariable(vm, name);
		}
		else if (symbols[0].first == SymbolType::Function) {
			//std::string string = name + "." + std::get<std::string>(symbols[0].second);
			//type = FindTypeForVariable(vm, (FunctionToken*)parent, name);
		}
	}
	for (auto c : children)
		c.second->PreProcess(vm);
	return restart;
}
Status ClassToken::GetCPP(VM& vm) {
	Status restart = CONTINUE;
	std::string n = "";
	//CPP temp = { "{\n" };
	for (auto c : children) {
		//if (c.second->token_type != THE_TOKEN_TYPE::FUNCTION) {
			c.second->GetCPP(vm);
			c.second->needs_compile = false;
		//}
	}
	for (auto c : children) {
		out_cpp += c.second->out_cpp;
	}
	//if (extends.size() > 0)
	//	n = fmt::format(class_extends_template, name, extends, "{}", "{", "}", temp);
	//else
	//	n = fmt::format(class_template, name, "{}", "{", "}", temp);
	//needs_compile = false;
	//out_cpp.body += "";
	return restart;

}
#define function_template "\
{0} {1}({2}){4}\n\
{3}\
{5}|||{6}|||{7}|||{8}|||{9}|||{10}|*|"
#define function_template2 "\
{0} {1}({2}){4}\n\
{3}\
{5}"
Status GetCPPLast(VM& vm, FunctionToken* parent, VariableToken* token) {
	auto status = token->GetCPP(vm);
	if (token->global) {
		//parent->type = {BasicType::nul, "void"};
		return status;
	}
	if (token->type.type_name == "void") {
		//parent->type = {BasicType::nul, "void"};
		return status;
	}
	if (token->out_cpp.in_out.size() > 3)
		if (token->out_cpp.in_out.substr(0, 3) == "ret" || token->out_cpp.in_out.substr(0, 3) == "swi")
			return status;

	token->out_cpp.in_out = "return " + token->out_cpp.in_out;
	/*if (status == RESTART)
		return status;
	int found_equals = last.body.find('='); //todo: this is hack, that equals could be in a string, fix asap
	if (found_equals > -1) {
		out_cpp += last;
		parent->type = { BasicType::nul, "void" };
		return status;
	}
	//if (parent->type == "")
		//parent->type = token->type;
	if (parent->type.type == BasicType::nul) {
		out_cpp += last;
		return status;
	}
	if (token->type.type != BasicType::nul) {
		if ((int)(last.body.find("switch")) > -1)
			out_cpp += last;
		else
			out_cpp.body += "return " + last.body;
		if (parent->type.type_name.size() == 0)
			parent->type = token->type;
		auto& label = vm.label(parent->mangled_name, {});
		label.SetType(parent->type);
		return status;
	}
	if (parent->type.type != BasicType::nul) {
		if (last.body.size() > 3)
			if (last.body[0] == 'r' && last.body[1] == 'e' && last.body[2] == 't') {
				out_cpp += last;
				return status;
			}
		out_cpp.body += "return " + last.body;
	}
	else
		out_cpp.body += "return " + last.body;*/
	return status;
}
std::string FunctionToken::MangleFunction(VM& vm) {
	auto test_type = type;
	if (test_type.type == BasicType::nul)
		test_type = { BasicType::nul, "void" };
	std::string mangled = "_" + test_type.type_name + "_" + name;
	for (auto arg : args) {
		if (arg.second.type_name.size() != 0)
			mangled += "_" + arg.second.type_name;
		else
			mangled += "_#";
	}
	return mangled;
	//vm.addTemplateFunction(name, mangled);
}
; typedef struct _Animal {
	uint64_t VTABLE;
	uint8_t age;
	uint64_t name;
}Animal;
Status FunctionToken::GetCPP(VM& vm) {
	if (!needs_compile) return CONTINUE;
	auto tttt0 = sizeof(uint64_t);
	auto tttt1 = sizeof(uint8_t);
	auto tttt = sizeof(Animal);
	Status restart = CONTINUE;
	std::string n = "";
	//std::string exports = "";
	std::string temp = "";
	std::string temp2 = "";
	std::string pre = "";
	std::string post = "";
	//Animal$Animal -> should be mangled_name not name?
	//otherwise split Function name from tokens
	//if name has $ then its a template constructor and class, so make sure the class and functions are built.
	if (name != "ffi") {
		if (auto got = vm.FindClass(name)) {
			if (!got->is_template) {
				//put the following in a GenConstructor function
				int size = 0;
				std::string temp4 = "";
				if (got->extends.size() > 0) {
					std::vector<class_def*> heirachy;
					heirachy.push_back(got);
					auto t = got;
					while (auto v = t->GetParentClass(vm)) {
						heirachy.push_back(v);
						t = v;
					}
					for (int i = heirachy.size() - 1; i > -1; --i) {
						for (auto v : heirachy[i]->variables)
							temp4 += "T$->" + v.first + " = " + MixedToString(v.second.value) + ";\n";
					}

				}
				//This is a constructor, add allocc, init variables and return object

				for (auto v : got->variables)
					temp4 += "T$->" + v.first + " = " + MixedToString(v.second.value) + ";\n";
				AddVariable(vm, "_u64_allocc_u64");
				AddClass(vm, name);
				pre += name + "* T$ = _u64_allocc_u64(sizeof(" + name + "));\n";
				pre += temp4;
				post = "return T$;\n";
			}
			else {
				return RESTART;
			}
		}
	}
	for (auto arg : args) {
		if (arg.second.type_name.size() == 0) {
			//Can't build this function yet, will be built once first called so we can get the type.
			return restart;
		}
		/*if (!BasicTypeCheck(arg.second)) {
			temp += arg.second.type_name + " $" + arg.first + ",";
			temp2 += "#define " + arg.first + " ($" + arg.first + ")\n"; //probrably better to declare a varaible here, ffi f = *$f;
			//temp2 += "#define " + arg.second.name + " (*$" + arg.second.name + ")\n"; //probrably better to declare a varaible here, ffi f = *$f;
		}
		else {*/
			if (Helper::BasicTypeCheck(arg.second.type_name))
				temp += arg.second.type_name + " " + arg.first + ",";
			else
				temp += arg.second.type_name + "* " + arg.first + ",";
		//}
	}
	auto& label = vm.label(mangled_name, {});

	if (args.size() > 0)
		temp.pop_back();

	//if (temp.size() > 0)
		//temp.pop_back();
	out_cpp.in_out = pre + temp2;
	int i = 0;
	for (auto c : children) {
		i++;
		if (i < children.size()) {
			auto status = c.second->GetCPP(vm);
			if (status == RESTART)
				return status;
			out_cpp.in_out += '\n';
		}
		else {
			auto status = GetCPPLast(vm, this, (VariableToken*)c.second);
			if (status == RESTART)
				return status;
		}
		out_cpp.in_out += c.second->out_cpp.in_out;
	}
	out_cpp.in_out += post;
	//if (is_template)
		//vm.template_functions[name][mangled_name].body = body;
	auto atLeastSpace = [](std::string& str) {
		if (str.size() == 0)
			str = " ";
	};
	auto complex = label.GetType();
	atLeastSpace(mangled_name);
	atLeastSpace(temp);
	atLeastSpace(complex.type_name);

	//ffi_get just returns a u32, the pointer to a function
	//on the left side is something like
	//add2(i32,i32):i32 the parser creates the definition in vm
	//combinging the two sets the pointer to the function
	//add2(i32,i32):i32 = ffi_get(lib, "add2")
	if (!broken) {
		if (!importing_func)
			if (name == "entry1") {
				n = fmt::format(function_template2, complex.GetComplexType(), mangled_name, temp, out_cpp.in_out, "{", "}");//, exports, mangled_name, label.ptr, _abs, classes
			}
			else
				n = fmt::format(function_template2, complex.GetComplexType(), mangled_name, temp, out_cpp.in_out, "{", "}");
		else {
			//instead of compiling this func, since its already compiled in library, do ?
			//n = fmt::format(function_template, label.GetComplexType(), name, temp, body, "{", "}", exports, name, label.ptr, _abs);
		}
	}
	if (out_cpp.in_out.size() == 0)
		if (name == "entry1")
			return restart;
	out_cpp.functions[mangled_name] = { mangled_name, std::to_string(label.ptr), n };
	return restart;
}
bool isSingleVariable(const std::string &str) {
	bool hasDelims = !str.empty() && str.find_first_not_of(" ") == std::string::npos;
	if (!hasDelims) {
		return true;
	}
	else
		return false;
}
bool isNumber(const std::string& line) { char* p; strtol(line.c_str(), &p, 10); return *p == 0; }
class_def* Token::AddClass(VM& vm, std::string_view to_find, const std::vector<std::string>& tkns) {
	if (to_find == "$") return nullptr;
	if (auto got = vm.FindClass(to_find.data())) {
		out_cpp.classes[got->name] = got->get_def(vm, tkns);
		for (auto v : got->variables) {
			if (v.second.type == BasicType::var || v.second.type == BasicType::str) {
				if (auto c = vm.FindClass(v.second.GetType())) {
					out_cpp.classes[c->name] = c->get_def(vm);
				}
			}
		}
		return got;
	}
	return nullptr;
};
Status VariableToken::GetCPP(VM& vm) {
	Status restart = CONTINUE;
	if (!needs_compile)
		return restart;
	std::string temp = "";
	std::string values = "";
	std::string new_type = "";
	bool in_branch = false;
	bool endbranch = false;
	bool has_equals = false;
	bool setting_member = false;
	if (int(name.find("->")) > -1) {
		setting_member = true;
	}
	/*if (symbols.size() > 0) {
		DemangledStruct out_struct;
		if (!FindMostQualified(symbols, 0, name, parent, out_struct)) {
			printf("Error: Can't find any suitable function for %s.\n", name.data());
			return;
		}
		int variadic_start = 100;
		for (uint16_t i = 0; i < out_struct.args.size(); ++i) {
			auto v = out_struct.args[i];
			if (v->basic == BasicType::variadic) {
				//Assign all parameters at this point and beyond, to a variadic
				variadic_start = i;
			}
		}
	}*/
	if (parent) {
		if (1) { //parent->name != "root"
			auto p = (FunctionToken*)parent;
			if (type.type_name.size() != 0) {
				//p->already_set.emplace(mangled_name);
				//type = new_type;
				AddVariable(vm, mangled_name);
			}
			//if past variadic, then make sure these variables get added to a variadic and then append the variadic to the arguments
			for (auto n = 0; n < this->symbols.size(); ++n) {
				endbranch = false;
				auto& a = this->symbols[n];
				if (IsType(a.first)) {
					//type = FindTypeForVariable(vm, (FunctionToken*)parent, std::get<std::string>(v.second));
					auto temp_type = mixedToType2(vm, a);
					if (type.type_name.size() == 0)
						type = temp_type;
					if (a.first == SymbolType::Variable) {
						auto split = Helper::split_1(MixedToString(a.second), '.');
						if (split.size() > 1)
							values += split[0] + "->" + split[1];
						else
							values += split[0];
					} 
					else
						if (a.first != SymbolType::Type)
							values += MixedToString(a.second);
					//if (a.first != SymbolType::Char) {
						AddVariable(vm, MixedToString(a.second));
						AddClass(vm, temp_type.type_name);
						//auto& b = this->symbols[n+1];
						//if (b.first)
					//}
				}
				else if (a.first == SymbolType::Minus)
					values += "-";
				else if (a.first == SymbolType::Else) {
					values += "else {";
					if (symbols.size() > n + 1)
						if (symbols[n + 1].first == SymbolType::Branch)
							values.pop_back();
				}
				else if (a.first == SymbolType::Branch) {
					values += "if (";
					in_branch = true;
				}
				else if (a.first == SymbolType::Switch) {
					values += "switch (";
					in_branch = true;
				}
				else if (a.first == SymbolType::CastAs) {
					auto str = std::get<std::string>(a.second);
					values += "(" + str + "*)(";
				}
				else if (a.first == SymbolType::ParamPack) {
					auto& a = this->symbols[n+1];
					if (a.first != SymbolType::ArgsEnd) {
						printf("Error: Parameter pack must be at the end of the function.");
						continue;
					}
					else {
						values += "int count = 0, ...";
					}
				}
				else if (a.first == SymbolType::Case) {
					std::string v = "";
					if (this->symbols[n + 1].first == SymbolType::Char) {
						v = '\'' + std::get<std::string>(this->symbols[n + 1].second) + '\'';
						n++;
					}
					else if (this->symbols[n + 1].first >= SymbolType::U8 && this->symbols[n + 1].first <= SymbolType::U64) {
						v = std::to_string(std::get<std::uint64_t>(this->symbols[n + 1].second));
						n++;
					}
					else if (this->symbols[n + 1].first == SymbolType::Variable) {
						v = std::get<std::string>(this->symbols[n + 1].second);
						auto split = Helper::split_1(v, '.');
						std::string left = "";
						std::string right = "";
						if (split.size() > 1) {
							left = split[0];
							right = split[1];
							//This can be a function
							if (AddClass(vm, left))
								v = right + "_" + left;
						}
						else {
							left = v;
							AddVariable(vm, left);
						}
						n++;
					}
					else {
						printf("Wrong type for switch.\n");
					}
					values += "case " + v + ":{";
					//in_branch = true;
				}
				else if (a.first == SymbolType::While) {
					values += "while (";
					in_branch = true;
				}
				else if (a.first == SymbolType::Default) {
					values += "default :{";
					in_branch = false;
				}
				else if (a.first == SymbolType::EndBranch) {
					values += ";}\n//EndBranch\n";
					endbranch = true;
					//in_branch = false;
				}
				else if (a.first == SymbolType::AngleBracketLeft) {
					values += "<";
				}
				else if (a.first == SymbolType::AngleBracketRight) {
					values += ">";
				}
				else if (a.first == SymbolType::False) {
					values += "0";
				}
				else if (a.first == SymbolType::True) {
					values += "1";
				}
				else if (a.first == SymbolType::Return) {
					if (type.type == BasicType::template_type)
						type = { BasicType::nul, "void" };
					values += "return ";
				}
				else if (a.first == SymbolType::ParenLeft) {
				values += "(";
				}
				else if (a.first == SymbolType::ParenRight) {
				values += ")";
				}
				else if (a.first == SymbolType::BracketLeft) {
					values += "[";
				}
				else if (a.first == SymbolType::BracketRight) {
					values += "]";
				}
				else if (a.first == SymbolType::Equals) {
					has_equals = true;
					values += "=";
				}
				else if (a.first == SymbolType::Plus) {
				values += "+";
				}
				else if (a.first == SymbolType::Star) {
				values += "*";
				}
				else if (a.first == SymbolType::Member) {
					auto token = vm.labels.find(name);
					if (token != vm.labels.end()) {
						AddVariable(vm, name);
						if (token->second.GetType().type_name.size() != 0) {
							AddClass(vm, token->second.GetType().type_name);
						}
					}
					name += "->" + std::get<std::string>(a.second);
					mangled_name = name;
					setting_member = true;
				}
				else if (a.first == SymbolType::Variable) {
					auto f = std::get<std::string>(a.second);
					auto split = Helper::split_1(f, '.');
					std::string left = "";
					std::string right = "";
					if (split.size() > 1) {
						left = split[0];
						right = split[1];
					}
					else
						left = f;
					auto type = FindTypeForVariable(vm, left);
					/*auto token = vm.labels.find(left);
					if (token != vm.labels.end()) {
						auto got = p->already_set.find(left);
						if (got == p->already_set.end()) {
							p->already_set.emplace(left);
							if (type.length() == 0)
								new_type = token->second.GetType();
							p->exports += token->second.def;
						}
						if (token->second.GetType().size() != 0) {
							auto got = p->already_set.find(token->second.GetType());
							if (got == p->already_set.end()) {
								p->already_set.emplace(token->second.GetType());
								auto token1 = vm.class_defs.find(token->second.GetType());
								if (token1 != vm.class_defs.end()) {
									p->classes += token1->second.get_def();
								}
								else
									printf("Unable to find type:%s ERRCODE:300 \n", token->second.GetType().data());
							}
							if (right.size() > 0)
								left = left + "->" + right;
						}
					}
					else {
						auto func = vm.functions.find(left);
						if (func != vm.functions.end()) {
							if (func->second.size() > 1) {
								printf("Too many versions of %s, pick one ->\n", left.data());
								for (auto ff : func->second)
									printf("  %s\n", ff->name.data());
							}
							else {
								auto got = p->already_set.find(left);
								if (got == p->already_set.end()) {
									p->already_set.emplace(left);
									if (type.length() == 0)
										new_type = func->second[0]->GetType();
									p->exports += func->second[0]->def;
									left = func->second[0]->name;
								}
							}
						}
						else {
							auto g = p->locals.find(left);
							if (g != p->locals.end()) {
								left = left + "->" + right;
								auto got2 = vm.class_defs.find(g->second);
								if (got2 != vm.class_defs.end()) {
									p->classes += got2->second.get_def();
								}
								else
									printf("Unable to find type:%s ERRCODE:301d\n", left.data());
							}
							if (right.size() > 0) {
								if (auto got1 = AddClass(vm, left)) {
									left = left + "$" + right;
									if (type.size() == 0) {
										if (got1->is_enum)
											type = got1->name;
										//else get type for right
									}
								}
								else {
									auto got = vm.labels.find(left);
									if (got != vm.labels.end()) {
										auto got2 = vm.class_defs.find(got->second.GetRealType());
										if (got2 != vm.class_defs.end()) {
											p->classes += got2->second.get_def();
											left = left + "->" + right;
											/*auto got3 = got2->second.variables.find(right);
											if (got3 != got2->second.variables.end()) {
												args += mixedToType(got3->second) + "_";
											}*/
										/*}
										else {
											printf("Unable to find type:%s ERRCODE:301b\n", left.data());
										}

									}
									else {
										auto g = p->args.find(left);
										if (g != p->args.end()) {
											left = left + "->" + right;
											auto got2 = vm.class_defs.find(g->second.type);
											if (got2 != vm.class_defs.end()) {
											}
											else
												printf("Unable to find type:%s ERRCODE:301c\n", left.data());
										}
										else {
											printf("Unable to find type:%s ERRCODE:301\n", left.data());
										}
									}
								}
							}
						}
					}*/
					AddVariable(vm, left);
					AddClass(vm, type.type_name);
					if (!Helper::BasicTypeCheck(type)) {
						if (split.size() > 1)
							values += left + "->" + right;
						else
							values += left;
					}
					else {
						values += f;
					}
				}
				else if (a.first == SymbolType::Function) {
					auto f = std::get<std::string>(a.second);
					DemangledStruct out_struct;
					if (!FindMostQualified(symbols, n, f, out_struct, restart)) {
						return restart;
					}
					if (restart == RESTART)
						return restart;
					AddVariable(vm, out_struct.mangled);
					AddClass(vm, out_struct.type.GetType());
					if (type.type_name.size() == 0) //hack, i think we need this
						type = out_struct.type;
					//Helper::replaceAll(f, ":", "_");
					values += out_struct.mangled;
					//type = out_struct.type;
					values += "(";
				}
				else if (a.first == SymbolType::For) {
				/*auto var = "i"; //i
				auto start = std::pair<SymbolType, Mixed>{ SymbolType::Variable, "0" }; //0
				auto end = std::pair<SymbolType, Mixed>{ SymbolType::Variable, "v.size" }; //v.size
				auto t = mixedToType2(vm, p, start);
				auto t1 = mixedToType2(vm, p, end);
				std::string out = t.type_name + var + ";" + var ;*/
				values += "for(u64 ";
				in_branch = true;

				}
				else if (a.first == SymbolType::ArrayStart) {
				//[1,2,1,1] -> i64$;

				std::vector<Type> types;
				bool end_braket_found = false;

				/*if (auto got = vm.FindClass(type.GetComplexType())) {
					AddClass(vm, type.type_name + "$");
				}
				else {
					class_def vector_def;
					vector_def.name = type.type_name + "$";
					vector_def.variables["size"] = { BasicType::u64, "u64", uint64_t(0) };
					vector_def.variables["capacity"] = { BasicType::u64, "u64", uint64_t(0) };
					vector_def.variables["data"] = type; //i8*
					vector_def.variables["data"].isNativeArray = true;
					vm.class_defs[type.type_name + "$"] = vector_def;
					vm.reserved_keywords[type.type_name + "$"] = SymbolType::Type;
					AddClass(vm, type.type_name + "$");
					AddClass(vm, "vector");
				}*/
				int ii = 0;
				Type _type;
				int type_count = 0;
				std::string temp_values = "";
				for (int i = n; i < symbols.size(); ++i) {
					if (IsType(symbols[i].first)) {
						auto temp_type = mixedToType2(vm, symbols[i]);
						if (_type.type != temp_type.type) {
							_type = temp_type;
							type_count++;
						}
						type = { BasicType::var, "vec$" + _type.GetType() };
						//add checks to make sure types fit array, else emit error
						//auto t = mixedToType2(vm, p, a);
						//int_vector.data[0] = 1;
						temp_values += name + "->data[" + std::to_string(ii++) + "] = " + MixedToString(symbols[i].second) + ";\n";
						//types.push_back(mixedToType2(vm, p, a));
					}
					else if (symbols[i].first == SymbolType::ArrayEnd) {
						end_braket_found = true;
						n += i - n;
						break;
					}

				}
				setting_member = true;
				if (has_equals) {
					auto _t = "_" + type.type_name + "_" + type.type_name + "_";
					AddVariable(vm, _t);
				}
				values += temp_values;
				if (IsType(symbols[n - 1].first)) {
					type.isArray = true;
					type.type = BasicType::var;
				}
				if (!end_braket_found)
					printf("Error: No matching bracket for array.\n");

				}
				else if (a.first == SymbolType::ArgsEnd) {
					values += ")";
				}
				else if (a.first == SymbolType::BranchArgsEnd) {
					values += "){";
				}
				else if (a.first == SymbolType::Comma) {
				values += ",";
				}
				else if (a.first == SymbolType::SizeOf) {
				auto isType = IsType(symbols[n+1].first);
				values += "sizeof(" + MixedToString(symbols[n+1].second);
				}
			}
				/*auto split = Helper::split_1(mangled_name, '.');
				if (split.size() > 1) {
					auto var_name = split[0];
					auto var_method = split[1];
					auto token = vm.labels.find(var_name);
					if (token != vm.labels.end())
						type = token->second.GetType();
					else {
						auto p = (FunctionToken*)parent;
						auto got = p->args.find(var_name);
						if (got != p->args.end()) {
							type = got->second;
						}
					}

				}
				else if (split.size() > 0) {
					auto var_name = split[0];
					auto token = vm.labels.find(var_name);
					if (token != vm.labels.end())
						type = token->second.GetType();
					else {
						auto p = (FunctionToken*)parent;
						auto got = p->locals.find(var_name);
						if (got != p->locals.end()) {
							type = { got->second,  };
						}
					}
				}*/
			//if (in_branch)
				//values += ';}';
			//if (setting_member)
				//type = "";
			std::string nl = ";\n";
			if (in_branch || endbranch)
				nl = "\n";
			if (name == "")
				temp = values + nl;
			else
				if (global) {
					AddVariable(vm, name);
					temp = mangled_name + values + nl;
				}
				else {
					if(IsVarSet(vm, name) || setting_member)
						temp = mangled_name + values + nl;
					else {
						AddVariable(vm, name);
						temp = type.GetComplexType() + " " + mangled_name + values + nl;
					}
				}
			out_cpp.in_out += temp;
		}
		else {
			//insert into vm as global

		}
		if (parent->name == "entry1") {
			//auto& label = vm.label(name); //reserve function in vm
			//label.type = type;
			//label.def = std::string("extern ") + type + " " + name + "(" + ");" + name + ";" + std::to_string(label.ptr) + "\n";
		}
	}
	for (auto c : children)
		c.second->GetCPP(vm);
	for (auto c : children) {
		out_cpp.in_out += c.second->out_cpp.in_out;
		//out_cpp += c.second->out_cpp;
	}
	return restart;
	//out_cpp.back() = '\n';
}
/*Token* ParseToken(UnparsedToken& token) {
	Token* out = nullptr;
	if (token.tokenType == CLASS) {
		out = new ClassToken(token);
	}
	else if (token.tokenType == FUNCTION) {
		out = new FunctionToken(token);
	}
	else if (token.tokenType == VARIABLE) {
		out = new VariableToken(token);
	}
	return out;
}*/

EnumToken::EnumToken(std::vector<std::pair<SymbolType, Mixed>>& symbols) {
	name = std::get<std::string>(symbols[1].second);
	symbols.erase(symbols.begin(), symbols.begin() + 3);
	symbols.erase(std::remove_if(symbols.begin(), symbols.end(), [](std::pair<SymbolType, Mixed> v) {return v.first == SymbolType::Comma; }), symbols.end());
	this->symbols = symbols;
	class_def def;
	def.name = name;
	def.is_enum = true;
	uint64_t c = 0;
	for (int i = 0; i < symbols.size(); ++i) {
		auto v = std::get<std::string>(symbols[i].second);
		if (symbols.size() > i + 1) {
			if (symbols[i + 1].first == SymbolType::Equals) {
				def.variables[v] = { BasicType::u8, "", std::get<std::uint64_t>(symbols[i + 2].second) };
				i += 2;
			}
			else
				def.variables[v] = { BasicType::u8, "", c };
		}
		else
			def.variables[v] = { BasicType::u8, "", c };
		c++;
	}
	auto& vm = Parsed::get()->getVM();
	vm.class_defs[name] = def;
}

void EnumToken::Print()
{
}

Status EnumToken::GetCPP(VM & vm) {
	//this->exports += "|1THISISATEST2|";
	return CONTINUE;
	//out_cpp += "|THISISATEST|";
}

Status EnumToken::PreProcess(VM & vm) {
	return CONTINUE;
}
