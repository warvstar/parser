#pragma once
#include "jit.h"
#include <iomanip>
#include <variant>
#include "tokens.h"
#include "helper.h"
#include <unordered_set>
#include "archive.h"
//technically we are writing an aot compiler for a vm, not a jit
//use relative positions
//null access values straight
//i32 access using dword
//i64 access using dword
enum class x32 {
	a, b, c
};
//s = non volatile
enum class x64 {
	a, b, c, d, e, f, s
};
enum class RegType {
	x32, x64
};
enum class By {
	val, ptr, ref
};
struct Label {
private:
	Type type = { BasicType::nul, "", (uint64_t)0 };
public:
	size_t ptr = 0;
	std::string name = "";
	std::string debug = "";
	By by = By::val;
	std::string def = "";
	BasicType basic = BasicType::var;
	int position = 0;
	~Label() {
		//free memory if label gets destroyed
	}
	void SetType(const Type& t) { type = t; };
	const Type& GetType() { return type; };
	void ret(VM& vm);
	void call_near(VM & vm, const std::string & func);
	void move_abs(size_t value);
	void call_far(VM & vm, const std::string & func);
	void move(VM& vm, int32_t value, x64 reg);
	void print(VM& vm);
	void getMachineCode(VM& vm, std::string& mc);
	template <class T>
	void Serialize(T& archive) {
		archive& ptr& name& debug& by&def&basic&position;
	}
	std::string Serialize(Serializer& serializer) {
		serializer.Up();
		serializer.Add(ptr);
		serializer.Add(name);
		serializer.Add(debug);
		serializer.Add((uint64_t)by);
		serializer.Add(def);
		serializer.Add((uint64_t)basic);
		serializer.Add((uint64_t)position);
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
struct nullobj {

};
enum Other {
	FirstFree, None
};

struct _reg {
	int val = -1;
	RegType type = RegType::x32;
	BasicType basic = BasicType::nul;
	_reg(int v, RegType t) {
		val = v;
		type = t;
	};
};
struct Func {
	std::string name = "";
	Label* func = nullptr;
	bool is_virtual = false;
	std::string body = "";
};
struct Serializer {
	std::string out = "";
	int level = 0;
	constexpr static const char* SerializedLevel(const int l) {
		switch (l) {
		case 0:
			return "|";
		case 1:
			return "||";
		case 2:
			return "|||";
		case 3:
			return "||||";
		case 4:
			return "|||||";
		case 5:
			return "||||||";
		case 6:
			return "|||||||";
		case 7:
			return "||||||||";
		}
		return "|";
	};
	void Add(const int64_t v) { out += std::to_string(v) + SerializedLevel(level); };
	void Add(const uint64_t v) { out += std::to_string(v) + SerializedLevel(level); };
	void Add(const double v) { out += std::to_string(v) + SerializedLevel(level); };
	void Add(const std::string& v) { out += v + SerializedLevel(level); };
	void Add(const Mixed& v) {
		using Mixed = std::variant<uint64_t, int64_t, double, SymbolType, std::string, struct Label*, struct Token*>;
		if (std::holds_alternative<Label*>(v)) {
			Add(std::get<Label*>(v)->Serialize(this));
		}
		else if (std::holds_alternative<std::string>(v)) {
			Add(std::get<std::string>(v));
		}
		else if (std::holds_alternative<uint64_t>(v)) {
			Add(std::get<uint64_t>(v));
		}
		else if (std::holds_alternative<int64_t>(v)) {
			Add(std::get<int64_t>(v));
		}
		else if (std::holds_alternative<double>(v)) {
			Add(std::get<double>(v));
		}
	};
	void Up() { level++; };
	void Down() { level--; };
};
struct Deserializer {
	std::string in = "";
	int level = 0;
	constexpr static const char* SerializedLevel(const int l) {
		switch (l) {
		case 0:
			return "|";
		case 1:
			return "||";
		case 2:
			return "|||";
		case 3:
			return "||||";
		case 4:
			return "|||||";
		case 5:
			return "||||||";
		case 6:
			return "|||||||";
		case 7:
			return "||||||||";
		}
		return "|";
	};
	std::vector<std::string> Split() {
		return Helper::split(in, SerializedLevel(level));
	}
	void Up() { level++; };
	void Down() { level--; };
};
struct class_def {
	std::string name = "";
	BasicType basic = BasicType::var;
	bool is_enum = false;
	bool is_template = false;
	tsl::ordered_map<std::string, Type> variables;
	std::unordered_map<std::string, Func> functions;
	std::string get_def(VM& vm, const std::vector<std::string>& template_vars = {});
	std::string add_types(VM& vm);
	std::string get_variables_def(VM& vm, const std::vector<std::string>& template_vars = {});
	tsl::ordered_map<std::string, Type> get_variables(VM& vm, const std::vector<std::string>& template_vars = {});
	class_def* GetParentClass(VM& vm);
	Type* FindVariable(VM& vm, const std::string& c, class_def* parent = nullptr);
	Func* FindFunction(VM& vm, const std::string& c, class_def* parent = nullptr);
	void AddFunction(VM& vm, Func func);
	void SetVirtual(VM& vm, const std::string& str);
	bool HasParent(VM& vm, const std::string& p);
	std::string extends = "";
	class_def* BuildClassFromTemplate(VM& vm, const std::string& str);
	std::string Serialize(Serializer& serializer) {
		serializer.Up();
		serializer.Add(name);
		serializer.Add((uint64_t)basic);
		serializer.Add((uint64_t)is_enum);
		serializer.Add((uint64_t)is_template);
		serializer.Up();
		for (auto v : variables) {
			serializer.Add(v.first);
			serializer.Add(v.first);
			v.second.Serialize(serializer);
		}
		serializer.Down();
		serializer.Add(extends);
		serializer.Down();
	};
	static class_def Deserialize(Deserializer& deserializer, const std::string& in2) {
		deserializer.Up();
		class_def l;
		auto in = deserializer.Split();
		l.name = in[0];
		l.basic = (BasicType)stoi(in[1]);
		l.is_enum = stoi(in[2]);
		l.is_template = stoi(in[3]);

		deserializer.Down();
		return l;
	}
};
struct mem_info {
	uint64_t start = 0;
	uint64_t end = 0;
};
struct template_class {
	std::string body = "";
	std::vector<std::string> args;
	std::string Serialize(Serializer& serializer) {
		serializer.Up();
		serializer.Add(body);
		serializer.Up();
		for (auto v : args)
			serializer.Add(v);
		serializer.Down();
		serializer.Down();
	};
	static template_class Deserialize(const std::string& in2) {
		template_class l;
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
class VM {
public:
	void Deserialize(const std::string& in2) {
		Deserializer deserializer;
		deserializer.Up(); 
		{
			auto in = deserializer.Split();
			for (int i = 0; i < in.size(); i += 2)
				labels[in[0]] = Label::Deserialize(in[1]);
		}
		deserializer.Down();

		deserializer.Up();
		{
			auto in = deserializer.Split();
			for (int i = 0; i < in.size(); i += 2)
				free_memory_positions.push_back({ stoi(in[i]),stoi(in[i + 1]) });
		}
		deserializer.Down();

		deserializer.Up();
		{
			auto in = deserializer.Split();
			deserializer.Up();
			auto in2 = deserializer.Split();
			for (int i = 0; i < in2.size(); i++)
				functions[in[0]].push_back(&labels.find(in2[i])->second);
			deserializer.Down();
		}
		deserializer.Down();

		deserializer.Up();
		{
			auto in = deserializer.Split();
			for (int i = 1; i < in.size(); i++)
				class_defs[in[0]] = class_def::Deserialize(in[1]);
		}
		deserializer.Down();
	}
	std::string Serialize() {
		Serializer serializer;
		serializer.Up();
		for (auto& v : labels) {
			serializer.Add(v.first);
			serializer.Add(v.second.Serialize(serializer));
		}
		serializer.Down();

		serializer.Up();
		for (auto& v : free_memory_positions) {
			serializer.Add(v.end);
			serializer.Add(v.start);
		}
		serializer.Down();
		
		serializer.Up();
		for (auto& v : functions) {
			serializer.Add(v.first);
			serializer.Up();
			for (auto vv : v.second)
				serializer.Add(vv->name);
			serializer.Down();
		}
		serializer.Down();

		serializer.Up();
		for (auto& v : class_defs) {
			serializer.Add(v.first);
			serializer.Add(v.second.Serialize(serializer));
		}
		serializer.Down();

		serializer.Up();
		for (auto& v : template_functions) {
			serializer.Add(v.first);
			serializer.Up();
			for (auto& vv : v.second) {
				serializer.Add(vv.first);
				serializer.Add(vv.second.Serialize(serializer));
			}
			serializer.Down();
		}
		serializer.Down();

		std::string data(mp.mem, mp.mem + mp.mem_size / 2);

		serializer.Add(data);

		return serializer.out;
	}
	std::unordered_map<std::string, std::string> macros;
	VM();
	std::vector<mem_info> free_memory_positions;
	std::vector<mem_info> free_memory_positions_vars;
	//have function to show memory fragmentation, just goes through all memory from 0 to max_size and checks with free_memory_positions to see whats not used
	intptr_t alloc(uint64_t size, uint64_t& pos);
	intptr_t alloc_var(uint64_t size, uint64_t & pos);
	void addMacro(const std::string & var, const std::string & val);
	std::string getMacros();
	void free(uint64_t start, uint64_t size);
	std::unordered_map<std::string, SymbolType> reserved_keywords = { {"sizeof", SymbolType::SizeOf}, {"bool", SymbolType::Type},{"false", SymbolType::False},{"true", SymbolType::True},{"f16", SymbolType::Type},{"f32", SymbolType::Type},{"f64", SymbolType::Type},{"i8", SymbolType::Type},{"i16", SymbolType::Type},{"i32", SymbolType::Type}, {"i64", SymbolType::Type}, {"u8", SymbolType::Type},{"u16", SymbolType::Type},{"u32", SymbolType::Type}, {"u64", SymbolType::Type}, {")", SymbolType::ArgsEnd},
{"import", SymbolType::Import}, {"class", SymbolType::Class},{"extends", SymbolType::Extends},{",", SymbolType::Comma},{"-", SymbolType::Minus},{"+", SymbolType::Plus},{"if", SymbolType::Branch},{"else", SymbolType::Else},{"while", SymbolType::While},{"switch", SymbolType::Switch},{"case", SymbolType::Case},{"default", SymbolType::Default},
{"return", SymbolType::Return}, {"<", SymbolType::AngleBracketLeft},{">", SymbolType::AngleBracketRight},{"enum", SymbolType::Enum},{"const", SymbolType::Const},{"*", SymbolType::Star},{"[", SymbolType::BracketLeft},{"]", SymbolType::BracketRight}, {"CastAs",SymbolType::CastAs}, {"TypeOf",SymbolType::TypeOf} , {"<",SymbolType::AngleBracketLeft},
	{">",SymbolType::AngleBracketRight}, {"for",SymbolType::For }
};
	MemoryPages mp;
	Label null_label;
	std::unordered_map<std::string, std::vector<Label*>> functions; //probably better to use a shared_ptr here
	std::unordered_map<std::string, class_def> class_defs = { {"i8", {"i8", BasicType::i8}},{"u8", {"u8", BasicType::u8}},{"i16", {"i16", BasicType::i16}},{"u16", {"u16", BasicType::u16}},{"i32", {"i32", BasicType::i32}},{"u32", {"u32", BasicType::u32}},{"i64", {"i64", BasicType::i64}},{"u64", {"u64", BasicType::u64}},{"f16", {"f16", BasicType::f16}},{"f32", {"f32", BasicType::f32}},{"f64", {"f64", BasicType::f64}},{"string", {"string"}},{"bool", {"bool", BasicType::Bool}},{"void", {"void", BasicType::nul}},{"#", {"#", BasicType::template_type}},{"$", {"$", BasicType::template_type}} }; //lookup vars here for classes, if found append to class and can find label from there
	std::unordered_map<std::string, Label> labels;
	std::unordered_map<std::string, std::unordered_map<std::string, template_class>> template_functions;
	void save();
	void append();
	void load();
	class_def* FindClass(const std::string& c) {
		auto got = class_defs.find(c);
		if (got != class_defs.end())
			return &got->second;
		else return nullptr;
	}
	//std::vector<std::string> globals;
	std::array<int, 8> registers32 = { 0,0,0,0,0,0,0,0 };
	std::array<int, 8> registers64 = { 0,0,0,0,0,0,0,0 };
	int debug_push_start = 0;
	std::string func = "";
	std::string debug_string = "";
	std::string debug_string_temp = "";
	void debug_push(const std::string& l);
	void debug_declare_label();
	void debug_endline();
	void addGlobal(const std::string& v) {
		auto t = "$" + v;
		auto f = labels.find(t);
		if (f == labels.end()) {
			auto& libName0 = label(t, v);
			libName0.def = "extern string* " + t + ";" + t + ";" + std::to_string(libName0.ptr) + "\n";
		}
	}
	void addGlobal(const uint64_t v) {
		auto t = "$" + std::to_string(v);
		auto f = labels.find(t);
		if (f == labels.end()) {
			if (fabs(v) > UINT32_MAX) {
				auto& libName = label(t, (uint64_t)v);
				libName.def = "extern u64 " + t + ";" + t + ";" + std::to_string(libName.ptr) + "\n";
			}
			else if (fabs(v) > UINT16_MAX) {
				auto& libName = label(t, (uint32_t)v);
				libName.def = "extern u32 " + t + ";" + t + ";" + std::to_string(libName.ptr) + "\n";
			}
			else if (fabs(v) > UINT8_MAX) {
				auto& libName = label(t, (uint16_t)v);
				libName.def = "extern u16 " + t + ";" + t + ";" + std::to_string(libName.ptr) + "\n";
			}
			else {
				auto& libName = label(t, (uint8_t)v);
				libName.def = "extern u8 " + t + ";" + t + ";" + std::to_string(libName.ptr) + "\n";
			}
		}
	}
	void addGlobal(const int64_t v) {
		auto t = "$" + std::to_string(v);
		auto f = labels.find(t);
		if (f == labels.end()) {
			if (fabs(v) > UINT32_MAX) {
				auto& libName = label(t, (int64_t)v);
				libName.def = "extern i64 " + t + ";" + t + ";" + std::to_string(libName.ptr) + "\n";
			}
			else if (fabs(v) > UINT16_MAX) {
				auto& libName = label(t, (int32_t)v);
				libName.def = "extern i32 " + t + ";" + t + ";" + std::to_string(libName.ptr) + "\n";
			}
			else if (fabs(v) > UINT8_MAX) {
				auto& libName = label(t, (int16_t)v);
				libName.def = "extern i16 " + t + ";" + t + ";" + std::to_string(libName.ptr) + "\n";
			}
			else {
				auto& libName = label(t, (int8_t)v);
				libName.def = "extern i8 " + t + ";" + t + ";" + std::to_string(libName.ptr) + "\n";
			}
		}
	}
	void addGlobal(const double v) {
		auto t = "$" + std::to_string(v);
		Helper::replaceAll(t, ".", "d");
		auto f = labels.find(t);
		if (f == labels.end()) {
			//globals.push_back(t); //could be used to track usage of globals, if not in use just omit
			if (fabs(v) > FLT_MAX) {
				auto& libName = label(t, (double)v);
				libName.def = "extern f64 " + t + ";" + t + ";" + std::to_string(libName.ptr) + "\n";
			}
			else {
				auto& libName = label(t, (float)v);
				libName.def = "extern f32 " + t + ";" + t + ";" + std::to_string(libName.ptr) + "\n";
			}
		}
	}
	int get_free_register_32();
	int get_free_register_64();
	uint32_t getBytesUsed();
	void printAllLabels();
	std::string getAllLabelsMachineCode();
	void preserve(x64 reg);
	void recover(x64 reg);
	void addTemplateFunction(const std::string & name, const std::string & mangled, const std::string & body, const std::vector<std::string>& args);
	//move reg into reg
	void move(x64 a, x64 b);
	void move(Label a, int reg = -1);
	void move_offset(Label a, int reg, int offset = 0);
	void move(x64 a, std::pair<x64, uint32_t> b);
	void move(std::pair<x64, uint32_t> a, x64 b);
	//does the same as the above two, just calls both
	void move(std::pair<x64, uint32_t> a, std::pair<x64, uint32_t> b);
	//move a value into a free register
	void move(size_t value);
	void move_i32(size_t value);
	void move_string(size_t value, int r = -1);
	//void move(Label& label);
	void move_abs(size_t value);
	//these are like move_far, they work on abs positions
	void move(std::string value, Label label);
	void move(int32_t value, Label label);
	void move(int64_t value, Label label);
	void move(int32_t value, x64 reg);
	void move(int64_t value, x64 reg);
	void move(uint64_t value, x64 reg);
	//move a register into a free register
	void move(_reg value);
	//moves a value into a ptr
	//void move(uint64_t value, size_t ptr);
	void move(uint8_t* value, size_t ptr);

	Label& create_i32(const std::string & name, int32_t v);

	//creates a ptr and assigns an i32 to it
	Label& create_ui64(const std::string & name, uint64_t v);
	Label& create_string(const std::string & name, const std::string& v);
	//adds two values at pointers
	void add_i32(uint8_t* a, uint8_t* b);
	//creates a blank pointer
	size_t create_ptr();
	//call a function
	void call_near(uint8_t* fn);
	void call_near_from(uint8_t * fn, int from);
	void call_far(size_t fn);
	//swap right to left
	void swap_reg32(int a, int b);
	void ret();
	void swap_reg64(int a, int b);
	void get_value(std::variant<size_t, std::string, Label, nullobj, _reg> v);
	template <class... Args>
	void for_each(Args&&... args) {
		using expand_type = int[];
		expand_type{ (get_value(args), 0)... };
	}
	template<typename ...Args>
	void call(const std::string& fn, Args&&... args) {
		for_each(args...);
		call(fn);
	}
	void ret(std::variant<Label, uint8_t, uint16_t, uint32_t, uint64_t, std::string> v);
	void call(x64 reg);
	void call(const std::string& fn);
	void pop();
	Label& label(const std::string& l, BasicType basic = BasicType::i32);
	Label& label(const std::string & l, std::vector<uint8_t> code);
	Label& label(const std::string& l, std::variant<uint8_t, uint16_t, uint32_t, uint64_t, int8_t, int16_t, int32_t, int64_t, double, float, std::string>);
	typedef std::variant<uint8_t, void*, const char*>(_fastcall *variant_func)(...);
	typedef std::any(_fastcall *any_func)(...);
	variant_func Run(const std::string& l);
	template <typename T>
	auto fetch_back(T& t) -> typename std::remove_reference<decltype(t.back())>::type
	{
		typename std::remove_reference<decltype(t.back())>::type ret = t.back();
		t.pop_back();
		return ret;
	}


	template <typename Ret, typename...Arg>
	Ret Run2(Ret(*func)(Arg...), std::vector<std::any> args) {
		if (sizeof...(Arg) != args.size())
			throw "Argument number mismatch!";

		return func(std::any_cast<Arg>(fetch_back(args))...);
	}
	/*template<typename T>
	auto Run(const std::string& l) {
		typedef T(_fastcall *print_func)(...);
		auto got = labels.find(l);
		if (got != labels.end()) {
			auto _print = reinterpret_cast<print_func>(&mp.mem[got->second.ptr]);
			return _print;
		}
		else {
			return DoNothing();
		}
	}*/
	void print();
	//returns ptr at label
	Label& get_label(const std::string& l);
	//returns ptr at register
	size_t get_register(const std::string& l);
	void clear_registers() {
		registers32.fill(0);
		registers64.fill(0);
	};
	//todo: implment const
	//run through the code, replace const funcs and loops with a const value
	void optimize();
	//repl
	//grow
	//class_def to code
	//func_def to code
	//variable_def to code
	//immediate_def to code - code not in blocks
	template<typename O, typename I>
	O ForceCast(I in) {
		union {
			I  in;
			O out;
		}
		u = { in };
		return u.out;
	};
};