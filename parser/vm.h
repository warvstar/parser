#pragma once
#include "jit.h"
#include <iomanip>
#include <variant>
#include "tokens.h"
//technically we are writing an aot compiler for a vm, not a jit
//use relative positions
//null access values straight
//i32 access using dword
//i64 access using dword
enum class BasicType {
	nul, i32, i64, str
};
struct Label {
	size_t ptr = 0;
	std::string type = "";
	BasicType basic = BasicType::i32;
	~Label() {
		//free memory if label gets destroyed
	}
};
struct VMObject {
	Label label;
	size_t start = 0;
	size_t end = 0;
	std::string name = "";
	std::string class_name = "";
	std::unordered_map<std::string, Label> variables; //these get new copys by object
	std::unordered_map<std::string, Label> functions; //these get referenced by object
};
struct VMClass {
	std::unordered_map<std::string, std::variant<Label, uint8_t, uint16_t, uint32_t, uint64_t, std::string>> variables; //default variables, not initialized
	std::unordered_map<std::string, Label> functions;
};
struct nullobj {

};
enum Other {
	FirstFree, None
};
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
struct _reg {
	int val = -1;
	RegType type = RegType::x32;
	BasicType basic = BasicType::nul;
	_reg(int v, RegType t) {
		val = v;
		type = t;
	};
};
struct VirtualReg {
	int order = 0;
	uint64_t id = 0;
	std::string name = "";
	SymbolType type = SymbolType::NA;
	std::variant<size_t, Label, _reg, nullobj> value;
	std::vector<VirtualReg*> depends_on;
	std::vector<VirtualReg*> dependant_on;
	VirtualReg* ret = nullptr;
	VirtualReg(int _order, uint64_t _id, const std::string& _name, std::variant<size_t, Label, _reg, nullobj> _value, std::vector<VirtualReg*> _depends_on = {}) {
		order = _order;
		id = _id;
		name = _name;
		value = _value;
		depends_on = _depends_on;
		for (auto d : depends_on)
			d->dependant_on.push_back(this);
	}
	VirtualReg() {};
};
struct VirtualHandle {
	std::variant<Label, VirtualReg> data;
};
struct CodeBlock {
	int order = 0;
	bool expects_return = false;
	std::vector<std::shared_ptr<VirtualReg>> registers;
	std::vector<std::shared_ptr<VirtualReg>> gets;
	std::vector<std::pair<bool, VirtualReg*>> all;
	void Optimize();
	void Add(const std::string& n, std::variant<size_t, Label, _reg, nullobj>);
	void EmptySet(const std::string& n, std::variant<size_t, Label, _reg, nullobj>);
	void Set(const std::string& n, std::variant<size_t, Label, _reg, nullobj>);
	void Set(const std::string& n);
	VirtualHandle Set(VirtualReg reg);
	VirtualHandle Set(Label label);
	void Get(const std::string & n, std::variant<size_t, Label, _reg, nullobj> v);
	void Call(const std::string & fn, std::pair<std::string, std::variant<Label, _reg, nullobj>> ret, Label & label, std::vector<std::pair<std::string, std::variant<size_t, Label, _reg, nullobj>>> args);
	void Print();
	void Ret();
};
class VM {
public:
	MemoryPages mp;
	uint8_t* data;
	Label null_label = { 0, "null" };
	std::unordered_map<std::string, Label> labels;
	std::unordered_map<std::string, VMObject> objects;
	std::unordered_map<std::string, VMClass> classes;
	std::array<int, 8> registers32 = { 0,0,0,0,0,0,0,0 };
	std::array<int, 8> registers64 = { 0,0,0,0,0,0,0,0 };
	int debug_push_start = 0;
	std::string func = "";
	std::string debug_string = "";
	std::string debug_string_temp = "";
	void debug_push(const std::string& l);
	void debug_declare_label();
	void debug_endline();
	int get_free_register_32();
	int get_free_register_64();
	VMObject& object(const VMClass& _class, const std::string& l);
	VMClass _class(const std::string& l);
	void AddBlock(CodeBlock& block);
	void preserve(x64 reg);
	void recover(x64 reg);
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

	//creates a ptr and assigns an i32 to it
	size_t create_i32(int32_t v);
	Label create_string(const std::string & name, const std::string& v);
	//adds two values at pointers
	void add_i32(uint8_t* a, uint8_t* b);
	//creates a blank pointer
	size_t create_ptr();
	//call a function
	void call_near(uint8_t* fn);
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
	Label label(const std::string& l, BasicType basic = BasicType::i32);
	Label label(const std::string& l, std::variant<uint8_t, uint16_t, uint32_t, uint64_t, std::string>);
	template<typename T>
	auto Run(const std::string& l) {
		typedef T(_fastcall *print_func)(...);
		auto _print = reinterpret_cast<print_func>(&mp.mem[labels[l].ptr]);
		return _print;
	}
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