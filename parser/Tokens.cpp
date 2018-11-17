#include "tokens.h"
#include "parser.h"
#include "helper.h"
#include "fmt/format.h"
#include "vm.h"
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
ClassToken::ClassToken(std::vector<std::pair<SymbolType, std::variant<uint64_t, int64_t, Func, SymbolType, std::string, Token*>>>& symbols) {
	name = std::get<std::string>(symbols[0].second);
	if (symbols.size() > 1)
		extends = std::get<std::string>(symbols[1].second);
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
FunctionToken::FunctionToken(std::vector<std::pair<SymbolType, std::variant<uint64_t, int64_t, Func, SymbolType, std::string, Token*>>>& symbols) {
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
int na_counter = 0;
VariableToken::VariableToken(std::vector<std::pair<SymbolType, std::variant<uint64_t, int64_t, Func, SymbolType, std::string, Token*>>>& symbols) {
	if (symbols[0].first == SymbolType::Variable) {
		name = std::get<std::string>(symbols[0].second);
		symbols.erase(symbols.begin());
	}
	if (name.size() == 0) {
		name = "na_" + std::to_string(na_counter++);
	}
	if (isdigit(name.front())) {
		name = "na_" + std::to_string(na_counter++);
	}
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
void ClassToken::GetCode(VM& vm) {
	type = "class";
	//vm.label(name);
	for (auto c : children) {
		c.second->GetCode(vm);
	}
}
void ClassToken::GetCode(CodeBlock & block, VM & vm) {

}
void FunctionToken::GetCode(VM& vm) {
	//vm.label(GetMangled());
	/*std::vector<std::string> args_for_registers;
	for (auto a : args) {
		if (a.name == "(" || a.name == ")" || a.name == ",")
			continue;
		else
			args_for_registers.push_back(a.name);
	}*/
	for (auto c : children) {
		c.second->GetCode(vm);
	}

	//if (name == parent->name) //this is a constructor, return ptr to class object
	//	vm.ret(vm.get_label(name));
}
void FunctionToken::GetCode(CodeBlock & block, VM & vm) {
}
std::string Token::GetMangled() {
	std::string out = "";
	if (parent)
		out += parent->GetMangled();
	if(name!="root")
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
void ClassToken::PreProcess(VM& vm) {
		for (auto c : children) {
			c.second->PreProcess(vm);
		}
}
void FunctionToken::PreProcess(VM& vm) {
	std::string name = "";
	std::string type = "";
	for (uint16_t i = 0; i < symbols.size(); ++i) {
		auto v = symbols[i];
		if (v.first == SymbolType::Variable) {
			if (name.size() > 0) {
				args[name] = { name, type };
			}
			name = std::get<std::string>(v.second);
		}
		else if (v.first == SymbolType::Type) {
			type = std::get<std::string>(v.second);
		}
		else if (v.first == SymbolType::ArgsEnd) {
			if (name.size() > 0) {
				args[name] = { name, type };

			}
		}
	}
	for (auto c : children) {
		c.second->PreProcess(vm);
	}
}
int stringcount = 0;
void VariableToken::PreProcess(VM& vm) {
	bool is_const = false;
	for (uint16_t i = 0; i < symbols.size(); ++i) {
		auto v = symbols[i];
		if (v.first == SymbolType::Plus) {
			if (i - 1 >= 0 && i + 1 < symbols.size()) {
				auto& last = symbols[i - 1];
				auto next = symbols[i + 1];
				if (last.first == SymbolType::UnsignedInt && next.first == SymbolType::UnsignedInt) {
					std::get<uint64_t>(last.second) += std::get<uint64_t>(next.second);
					symbols.erase(symbols.begin() + i);
					symbols.erase(symbols.begin() + i);
				}
			}

		}
		//add default values to class
		/*if (v.first == SymbolType::Variable) {
			auto tt = std::get<std::string>(v.second);
			if ()
		}
		else if (v.first == SymbolType::Plus) {

		}*/
	}
	for (uint16_t i = 0; i < value.size(); ++i) {
		auto v = value[i];
		if (v.front() == '"' && v.back() == '"') { //found string
			v.erase(0, 1);
			v.pop_back();
			auto n = "_S" + std::to_string(stringcount++);
			auto libName = vm.create_string(n, v);
			value[i] = n;
		}
	}
}
void ClassToken::GetCodeBlock(std::vector<CodeBlock>& blocks, VM& vm) {
	for (auto c : children) {
		c.second->GetCodeBlock(blocks, vm);
	}
}
void FunctionToken::GetCodeBlock(std::vector<CodeBlock>& blocks, VM& vm) {
	CodeBlock block;
	/*for (int i = 0; i < args.size(); ++i) {
		auto tt = args.nth(i);

	}*/
	bool expects_return = false;
	bool looking_for_return = false;
	for (auto a : symbols) {
		if (a.first == SymbolType::Variable) {
			auto tt = std::get<std::string>(a.second);
			block.Set(tt);
		} else if (a.first == SymbolType::ArgsEnd) {
			looking_for_return = true;
		}
		if (looking_for_return)
			if (a.first == SymbolType::Type)
				expects_return = true;
	}
	bool has_body = false;
	for (auto c : children) {
		c.second->GetCode(block, vm);
		has_body = true;
	}
	if (expects_return)
		if (has_body)
			block.Ret();
		else
			printf("Error: function requires return\n");
	blocks.push_back(block);
}
std::pair<std::string, std::variant<size_t, Label, _reg, nullobj>> GetType(VM& vm, std::pair<SymbolType, std::variant<uint64_t, int64_t, Func, SymbolType, std::string, Token*>> type, int index, FunctionToken* parent) {
	if (type.first == SymbolType::Variable || type.first == SymbolType::Function) { //found var or function
		auto n = std::get<std::string>(type.second);
		std::string nn = n;
		auto split1 = Helper::split(n, ".");
		if (split1.size() > 1) {
			nn = split1.front();
		}
		auto got = parent->args.find(nn);
		auto tt = parent->args.nth(index);
		//this is a passed arg, meaning its in one of the registers
		if (got != parent->args.end()) {
			//this is an arg(reg) value been passed down
			//if in proper order, nothing needs to be done
			if (tt == got) {
				auto type = got->second;
				if (isSmall(type.type))//any type representable by 32 bits
					return { n, _reg(index, RegType::x32) };
				else
					return { n, _reg(index, RegType::x64) };
			}
			//if out of order, then swap registers
			else {
				//todo
				int reg = 0;
				for (int i = 0; i < parent->args.size(); ++i) {
					tt = parent->args.nth(i);
					if (tt == got) {
						reg = i;
						break;
					}
				}
				//put reg into reg
				auto type = got->second;
				if (isSmall(type.type))//any type representable by 32 bits
					return { n, _reg(reg, RegType::x32) };
				else
					return { n, _reg(reg, RegType::x64) };
			}
		}
		auto parent_class = parent->parent;
		//if (!parent_class) parent_class = 
		auto found_var = parent_class->children.find(nn);
		if (found_var != parent_class->children.end()) {
			//get variable position from class definition
			//for now just use 0 offset, so good for one item, this will be handle
			//handle = dlopen(libName)
			//vm.move({ _reg(index + 1, RegType::x64), 0 }, _reg(index + 1, RegType::x64)); //mov rcx, qword ptr [rcx] this moves handle into rcx
			auto mangled = found_var->second->GetMangled();
			auto found_label = vm.labels.find(mangled);
			if (found_label != vm.labels.end()) {
				return { n, found_label->second };
			}
		}
		else {
			auto found_label = vm.labels.find(n);
			if (found_label != vm.labels.end()) {
				return { n, found_label->second };
			}
		}
		printf("Error: Could not find function %s", nn.data());
		return { n, nullobj() };
	}
	else if (type.first == SymbolType::Int) { //found var or function
		auto n = std::get<int64_t>(type.second);
		return { "some_int", n };
	}
	else if (type.first == SymbolType::UnsignedInt) { //found var or function
		auto n = std::get<uint64_t>(type.second);
		return { "some_int", n };
	}
	else if (type.first == SymbolType::String) { //found var or function
		auto n = std::get<std::string>(type.second);
		auto found_label = vm.labels.find(n);
		return { n, found_label->second };

	}
}
void VariableToken::GetCode(CodeBlock& block, VM& vm) {
	bool is_const = false;
	SymbolType op = SymbolType::NA;
	for (uint16_t i = 0; i < symbols.size(); ++i) {
		auto v = symbols[i];
		if (v.first == SymbolType::Variable) {
			std::string func_name = std::get<std::string>(v.second);
			std::vector<std::pair<std::string, std::variant<size_t, Label, _reg, nullobj>>> ret;
			auto split = Helper::split(func_name, ".");
			if (split.size() > 1) {
				//calling a class function, find variables class type
				auto found_classtype = parent->children.find(split[0]);

			}
			if (op == SymbolType::Plus) {
				auto t = GetType(vm, v, 0, (FunctionToken*)parent);
				block.Add(func_name, t.second);
			}
			auto split1 = Helper::split(name, ".");
			if (split1.size() > 1) {
				/*auto t = GetType(vm, split1[0], 0, (FunctionToken*)parent);
				if (std::holds_alternative<_reg>(t)) {
					auto tt = std::get<_reg>(t);
					block.Call(split[0], { name, tt }, vm.get_label(split[0]), ret);
				}
				else {
					block.Call(split[0], { "", nullobj() }, vm.get_label(split[0]), ret);
				}*/
			}
		}
		else if (v.first == SymbolType::Plus) {
			//tell block to expect to add next register with last
			op = SymbolType::Plus;
		}
		else if (v.first == SymbolType::UnsignedInt) {
			auto tt = std::get<uint64_t>(v.second);
			if (op == SymbolType::NA) {
				block.Get(std::to_string(tt), tt);
			}
			else if (op == SymbolType::Plus) {
				block.Add(std::to_string(tt), tt);
			}
			else if (op == SymbolType::Minus) {
				//block.Sub(std::to_string(tt), tt);
			}
		}
		else if (v.first == SymbolType::Function) {
			std::string func_name = std::get<std::string>(v.second);
			std::vector<std::pair<std::string, std::variant<size_t, Label, _reg, nullobj>>> args;
			int i22 = 0;
			for (uint16_t i2 = i + 1; i2 < symbols.size(); ++i2) {
				i22++;
				auto v2 = symbols[i2];
				if (v2.first == SymbolType::ArgsEnd) {
					break;
				}
				args.push_back(GetType(vm, v2, i, (FunctionToken*)parent));
			}
			auto t = GetType(vm, v, 0, (FunctionToken*)parent);
			if (std::holds_alternative<Label>(t.second)) {
				auto tt = std::get<Label>(t.second);
				block.Call(func_name, { name, tt }, vm.get_label(func_name), args);
			}
			else if (std::holds_alternative<_reg>(t.second)) {
				auto tt = std::get<_reg>(t.second);
				block.Call(func_name, { name, tt }, vm.get_label(func_name), args);
			}
			i += i22;
		}
		/*if (v == SymbolType::BracketLeft && i > 0) {
			std::string func_name = value[i - 1];
			std::vector<std::string> args;
			for (uint16_t i2 = i + 1; i2 < value.size(); ++i2) {
				auto vv = value[i2];
				if (vv[0] == ')')
					break;
				else if (vv[0] == ',')
					continue;
				else
					args.push_back(vv);

			}
			auto split = Helper::split(func_name, ".");
			if (split.size() > 1) {
				//calling a class function, find variables class type
				auto found_classtype = parent->children.find(split[0]);

			}
			else {
				//calling either a local variable or a global
				auto found_function = vm.labels.find(split[0]);
				if (found_function != vm.labels.end()) {
					std::vector<std::pair<std::string, std::variant<size_t, Label, _reg, nullobj>>> ret;
					for (int i = 0; i < args.size(); ++i)
						ret.push_back({ args[i], GetType(vm, args[i], i, (FunctionToken*)parent) });
					auto split1 = Helper::split(name, ".");
					if (split1.size() > 1) {
						auto t = GetType(vm, split1[0], 0, (FunctionToken*)parent);
						if (std::holds_alternative<_reg>(t)) {
							auto tt = std::get<_reg>(t);
							block.Call(split[0], { name, tt }, vm.get_label(split[0]), ret);
						}
						else {
							block.Call(split[0], { "", nullobj() }, vm.get_label(split[0]), ret);
						}
					}
					else {
						if (name.size() > 0) {
							auto t = GetType(vm, name, 0, (FunctionToken*)parent);
							if (std::holds_alternative<Label>(t)) {
								auto tt = std::get<Label>(t);
								block.Call(split[0], { name, tt }, vm.get_label(split[0]), ret);
							}
							else {
								block.Call(split[0], { "", nullobj() }, vm.get_label(split[0]), ret);
							}
						}
						else 
							block.Call(split[0], { "", nullobj() }, vm.get_label(split[0]), ret);
					}
					
					//vm.call(found_function->second.ptr); //allow to call by ptr later
				}
			}
			//vm.call();
		}
		else if (v.back() == '!') {
			std::string func_name = value[i - 1];

		}
		else if (v.front() == '@') {

		}*/
	}
}
void VariableToken::GetCodeBlock(std::vector<CodeBlock>& blocks, VM& vm) {
	GetCode(vm);
}

void VariableToken::GetCode(VM& vm) {
	//a silly hack, fix asap
	if (value.size() == 0 && type.size() != 0) {
		value.push_back(type);
	}
	std::vector<std::string> split_expr;
	std::string temp = "";
	std::unordered_map<std::string, FunctionToken*>::iterator got;
	std::unordered_map<std::string, VariableToken*>::iterator got_var;
	std::vector<std::pair<std::string, int>> split_expr_1;
	bool combine_next = false;
	int8_t args_needed = 0;
	auto mangled = GetMangled();
	if (parent->type == "class" && parent->name != "root") {
		//this is a variable declared in a class, do not initialize it here, but wait until object is created
		return;
	}

	/*auto got = parent->children.find(name);
	if (got != parent->children.end()) {

	}
	else {

	}*/
	//auto parent_man = mangle(this);
	//check if variable is set in scope and outerscopes
	//auto mangled = GetInAnyScope(name);
	//auto got = vm.labels.find(parent->GetMangled());
	//if not, variable is declared in scope
	//auto got = vm.labels.find(parent->GetMangled());
	/*if (got != vm.labels.end()) {

	}
	else {
		vm[name] = ;
	}*/
	for (uint16_t i = 0; i < value.size(); ++i) {
		auto v = value[i];
		if (v[0] == '(' && i > 0) {
			std::string func_name = value[i-1];
			std::vector<std::string> args;
			for (uint16_t i2 = i+1; i2 < value.size(); ++i2) {
				auto vv = value[i2];
				if (vv[0] == ')')
					break;
				else if (vv[0] == ',')
					continue;
				else
					args.push_back(vv);

			}
			auto split = Helper::split(func_name, ".");
			if (split.size() > 1) {
				//calling a class function, find variables class type
				auto found_classtype = parent->children.find(split[0]);

			}
			else {
				//calling either a local variable or a global
				auto found_function = vm.labels.find(split[0]);
				if (found_function != vm.labels.end()) {
					//if (args.size() == 0)
					//vm.call(split[0]);
					//else if (args.size() == 1) {
					//	vm.call(split[0], GetType(vm, args[0], 0, (FunctionToken*)parent));
					//}
					//else if (args.size() == 2)
						//vm.call(split[0], GetType(vm, args[0], 0, (FunctionToken*)parent), GetType(vm, args[1], 1, (FunctionToken*)parent));
					//vm.call(found_function->second.ptr); //allow to call by ptr later
				}
			}
			//vm.call();
		}
		else if (v.back() == '!') {
			std::string func_name = value[i - 1];

		}
		else if (v.front() == '@') {

		}
		else if (v.front() == '=') {
			std::string val = value[i + 1];
			if (name.size() > 0) {
				auto mangled = GetMangled();
				std::variant<uint8_t, uint16_t, uint32_t, uint64_t, std::string> _v;
				//make sure the type is allowed
				if (val.front() == '"' && val.back() == '"') { //found string
				}
				else if (isdigit(val.front())) { //found int
					_v = val;
				}
				else if (isalpha(val.front())) { //found variable or function
					if (value.size() >= i + 2) {
						if (value[i + 2] == "(") { //found function
							//get label
						}
					}
				}
				vm.label(mangled, _v);

				//vm.set_i32();
			}
		}
	}
	/*if (name.size() > 0) {
		auto mangled = GetMangled();
		vm.label(mangled);
	}*/
	/*auto got1 = vm.labels.find(mangled);
	if (got1 != vm.labels.end()) {
		//var already declared, reassigning it
	}
	else {
		//new var
		vm.label(mangled, );
	}*/
}
void ClassToken::GetLua(std::string& out_lua) {
	std::string n = "";
	std::string temp = "{\n";
	for (auto c : children) {
		c.second->GetLua(temp);
		temp += ",\n";
	}
	temp += '}';
	if (extends.size() > 0)
		n = fmt::format(class_extends_template, name, extends, "{}", "{", "}", temp);
	else
		n = fmt::format(class_template, name, "{}", "{", "}", temp);

	out_lua += n;

}
#define function_template "\
{0} = function{1}\n\
{2}\n\
end"
void GetLuaLast(FunctionToken* parent, VariableToken* token, std::string& out_lua) {
	std::string last = "";
	token->GetLua(last);
	if (parent->type != "void" && parent->type != "") {
		if (last.size() > 3)
			if (last[0] == 'r' && last[1] == 'e' && last[2] == 't') {
				out_lua += last;
				return;
			}
		out_lua += "return " + last;
	}
	else
		out_lua += last;
}
void FunctionToken::GetLua(std::string& out_lua) {
	std::string n = "";
	std::string temp = "";
	for (auto arg : args) {
		temp += arg.second.name;
	}
	if (args.size() == 0) {
		temp = "(self)";
	}
	else
		temp.insert(1, std::string("self,"));

	//if (temp.size() > 0)
		//temp.pop_back();
	std::string body = "";
	int i = 0;
	for (auto c : children) {
		i++;
		if (i < children.size() - 1) {
			c.second->GetLua(body);
			body += '\n';
		}
		else
			GetLuaLast(this, (VariableToken*)c.second, body);
	}
	n = fmt::format(function_template, name, temp, body);
	out_lua += n;
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

void VariableToken::GetLua(std::string& out_lua) {
	//a silly hack, fix asap
	if (value.size() == 0) {
		value.push_back(type);
	}
	//if (level == 0)
		//out_lua += "local " + name;
	//else
	out_lua += name;
	std::vector<std::string> split_expr;
	std::string temp = "";
	//v = 1
	//v = 1 + 1
	//v = add(getInt("1"), getInt("1 + 1") + 1)
	//v = add getInt "1" getInt "1 + 1" + 1
	//v = add(v0, getInt() + 1)
	//=
	//add
	//v0
	//getInt
	//+
	//1
	std::vector<std::string> test_vec = {
		"=",
		"add",
		"(",
		"getInt",
		"(",
		"\"1\"",
		")",
		",",
		"getInt",
		"(",
		"\"1 + 1\"",
		")",
		"+",
		"1",
		")" };
	std::unordered_map<std::string, FunctionToken*>::iterator got;
	std::unordered_map<std::string, VariableToken*>::iterator got_var;
	std::vector<std::pair<std::string, int>> split_expr_1;
	bool combine_next = false;
	int8_t args_needed = 0;
	for (uint16_t i = 0; i < value.size(); ++i) {
		auto v = value[i];
		//if (v == "+" || v == "-" || v == "=" || '(' || ')') {
		if (v[0] == '[') {
			v[0] = '{';
			if (v.back() == ']') {
				v.back() = '}';
			}
		}
		else if (v[0] == '"') {

		}
		else if (v.back() == '!') {
			Helper::replaceAll(v, ".", ":");
			v.pop_back(); v += "()";
		}
		else if (v.front() == '@') {
			v.front() = 's'; v.insert(1, "elf.");
		}
		else if (v == "super") {
			FunctionToken* p = (FunctionToken*)parent;
			if (p)
				v = "_parent_0." + p->name;
		}
		else {
			Helper::replaceAll(v, ".", ":");
		}
		temp += v;

		if (v.size() == 6)
			if (v == "return")
				temp += " ";
		//} else {
		//	temp += v + ',';

		//}
	}

	/*for (uint16_t i = 0; i < test_vec.size(); ++i) {
		auto v = test_vec[i];
		if (v == "+" || v == "-" || v == "=") {
			temp += v;
			combine_next = true;
		}
		else {
			auto g = Parsed::get();
			got = g->functions.find(v);
			if (got != Parsed::get()->functions.end()) {
				temp += "{}";
				std::string t = v + "(";
				int8_t args_need = 0;
				for (auto a : got->second->args) {
					args_need++;
					t += "{}";
				}
				t += ")";
				fill_args(out_lua, test_vec, {t, args_need}, i);
			}
			if (combine_next) {
				combine_next = false;
				temp += v;
			}
			else {
				temp += v + ',';
				split_expr.push_back(temp);
				if (split_expr.size() == 1)
					split_expr.push_back("(");

				//temp = "";
			}

		}
	}*/
	/*for (auto v : value) {
		if (v == "+") {
			temp.back() = '+';
			out_lua += temp;
		}
		else if (v == "=") {
			temp += '=';
		}
		else {
			auto g = Parsed::get();
			got = g->functions.find(v);
			if (got != Parsed::get()->functions.end()) {
				temp += v + '(';
				open_args = true;
				args_needed = got->second->args.size();
			}
			else {
				got_var = Parsed::get()->variables.find(v);
				if (got_var != Parsed::get()->variables.end()) {

				}
				else {
					//v is a basic, could be int, string...
					//check if int or string, if neither its an undefined symbol
					if (v[0] == '"' || v[0] == '[' || isNumber(v)) {
						if (args_needed < 2 && args_needed > -1)
							temp += v;
						else
							temp += v + ',';
						args_needed--;
						if (args_needed == 0 && open_args) {
							open_args = false;
							temp += ')';
						}
					}
					else {
						printf("Error: unknown symbol (%s)", v.data());
					}

				}
			}
		}
	}*/
	out_lua += temp;
	//out_lua.back() = '\n';
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