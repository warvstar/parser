//convert all to import later
#include "helper.h"
#include "parser.h"
#include "tokens.h"
#include "vm.h"
std::unique_ptr<Parsed> Parsed::instance;
std::once_flag Parsed::onceFlag;

//std::vector<Class> classes;
void Parse(std::string_view source) {
	//classes.push_back({ "main", {}, {} });
	auto lines = Helper::split_string(source, "\n");
	std::unique_ptr<ClassToken> root = std::make_unique<ClassToken>();
	root->name = "root"; root->level = 0;
	std::vector<Token*> indices;
	Token* last = root.get();
	int last_id = 0;
	auto& tokens = Parsed::get()->tokens;
	tokens.push_back(std::move(root));
	for (auto l : lines) {
		if (l.second < 2) 
			continue;
		if (Helper::splitTokens(tokens, source, l.first, l.second)) {
			auto token = tokens.back().get();
			int spaces = token->level - last->level;
			if (spaces > 0) {
				indices.push_back(last);
			}
			else while (spaces++ < 0)
				indices.pop_back();
			indices.back()->children[token->name] = token;
			token->parent = indices.back();
			last = indices.back()->children.back().second;
		}
	}
	tokens.front()->level = 0;
	//parsed->Print();
	Parsed::get()->root = tokens.front().get();
}
namespace Parser {
	std::string getLua(std::string_view source) {
		Parse(source);
		auto parsed2 = Parsed::get();
		//UnparsedToken temp;
		//auto print = new FunctionToken(temp);
		//auto add = new FunctionToken(temp);
		//auto getInt = new FunctionToken(temp);
		//print->name = "print";
		//add->name = "add";
		//getInt->args.push_back({ "a", "int", "" });
		//parsed2->functions["print"] = print;
		std::string out_lua = "";
		auto tt = (ClassToken*)parsed2->root;
		for (auto t : tt->children) {
			t.second->GetLua(out_lua);
			out_lua += '\n';
		}
		return out_lua;
	}

	std::string getCpp(std::string_view source) {
		Parse(source);
		auto parsed2 = Parsed::get();
		//UnparsedToken temp;
		//auto print = new FunctionToken(temp);
		//auto add = new FunctionToken(temp);
		//auto getInt = new FunctionToken(temp);
		//print->name = "print";
		//add->name = "add";
		//getInt->args.push_back({ "a", "int", "" });
		//parsed2->functions["print"] = print;
		std::string out_lua = "";
		auto tt = (ClassToken*)parsed2->root;
		for (auto t : tt->children) {
			t.second->GetLua(out_lua);
			out_lua += '\n';
		}
		return out_lua;
	}
	struct Lib {
		size_t handle = 0;
		size_t get(std::string_view fn) {
			return 1;
		}
	};
	Lib ffi(std::string_view libName) {
		return { 1 };
	}
	void supercooool(std::string_view c) {
		printf("msg:%s\n", c.data());
	};
	template<typename O, typename I>
	O ForceCast(I in) {
		union {
			I  in;
			O out;
		}
		u = { in };
		return u.out;
	};
	struct ClassDef {
		std::unordered_map<std::string, size_t> functions;
		std::unordered_map<std::string, size_t> variables;
	};
	/*
class ffi
 handle:ui64 = -1
 __create(libName:str):ffi:
  handle = dlopen(str)
 get(funcName:str):ui64
  dlsym(handle, funcName)

lib = ffi("somelib.dll")
printOne:void = lib.get(lib, "printOne")
printOne!

Step1:
 handle:ui64 = -1
 //put in machine code and get back pointer, add pointer to Classes["ffi"].variables["handle"]
 //done
Step2:
 __create(libName:str):ffi:
  handle = dlopen(str)
 //add a _createTest function, should be mangled with symbols so can overload
 //add function to class[] map

*/

	//Get a C++ class constructed in assembly, then continue with the steps above
	//add machine code for operator new
	//jmp QWORD PTR [rip+0x200bc2] # 601018 <operator new(unsigned long)@GLIBCXX_3.4>
	//push 0x0
	//jmp 400440 < .plt >
	//
	//Add machine code for Test()
	//push rbp
	//mov rbp, rsp
	//mov QWORD PTR[rbp - 0x8], rdi
	//nop
	//pop rbp
	//ret
	//Test* t = new Test();
	//mov edi, 0x8
	//call 400450 < operator new(unsigned long)@plt> //call the new function we created functions["new"]
	//mov rbx, rax
	//mov rdi, rbx
	//call 4005aa <Test::Test()> //call the Test() function we created classes["Test"].functions["Test"]
	//mov QWORD PTR[rbp - 0x18], rbx
	//copy from c++ - std::unique_ptr is a smart pointer that owns and manages another object through a pointer and disposes of that object when the unique_ptr goes out of scope. 
	//ref counting can be done later, for now just provide a function to delete/free ptrs/memory
	//actually ref counting may not even be needed, as can
	//void unique(void* ptr) {
		//reference counted ptr
		//exists in scope it was created
		//for example
		//objects:_u(Animal)<> //[] = array, <> = vector
		//createObject:: //exists in this function scope
		//a = unique(Animal!)
		//objects.push_back(move(a)) //exists in this vector now, and no where else as it has been moved
		//auto u = ptr();// std::unique_ptr<Test>();
	//}
	//use unique as default?
	//currently this should work
	//a_function::
	// a = 2
	//some_other_func:: //a goes out of scope here, is a simple type and does not need memory management
	// dog = Animal! //create a ptr with all the function ptrs for Animal
	//dog goes out of scope on function exit and gets deleted
	//shared_objects:_s(Animal)<>
	//another_func::
	// dog = Animal!
	// objects.push_back(move(dog)) //exists in this vector now, and no where else as it has been moved
	// cat = _s(Animal!) //count set to 1
	// shared_objects.push_back(dog) //count set to 2
	//function exit, cat counter decrements by 1, dog exists in objects vector and cat exists in shared_object vector
	//shared is easy to implement, unique is more of a challenge
	//unique should acually be easy too, just move the data to a new locataion and can still free memory at old location
	class TestClass {
	public:
		void print() { printf("working"); };
	};
	//int is only 32 bits, so for it to go into a, it needs to go into eax
	void dlopen(const char* c, const char* c2) {
		//typedef size_t(_fastcall *funv2)(void*);
		//auto _create = reinterpret_cast<funv2>(ForceCast<uint8_t*>(lib));
		//auto _create2 = reinterpret_cast<funv2>(ForceCast<uint8_t*>(lib2));
		//_create(lib);
		//return 1;
	}
	size_t dlsym(size_t handle, const char* c) {
		return 1;
	}
	int test_int = 2;
	std::string getMachineCode(std::string_view source) {
		VM vm;
		vm.labels["dlopen"].ptr = (size_t)dlopen;
		vm.labels["dlsym"].ptr = (size_t)dlsym;
		Parse(source);
		auto parsed2 = Parsed::get();
		printf("1");
		auto tt = (ClassToken*)parsed2->root;
		for (auto p : tt->children) {
			p.second->PreProcess(vm);
		}
		std::vector<CodeBlock> blocks;
		//for (auto p : tt->children) {
		tt->children.nth(1)->second->GetCodeBlock(blocks, vm);
		//tt->children.nth(1)->second->GetCodeBlock(blocks, vm);
			//p.second->GetCodeBlock(blocks, vm);
		//}
		blocks[0].Print();
		blocks[0].Optimize();
		//optimize blocks, convert virtual registers to actual
		for (auto block : blocks) {
			//block.Optimize();
			vm.AddBlock(block);
		}
		size_t i2 = vm.Run<size_t>("entry")();

		//name handle
		//type i64
		//value -1
		/*std::unordered_map<std::string, size_t> functions;
		std::unordered_map<std::string, ClassDef> classes;
		void* (*mpff)(size_t) = &::operator new;
		functions["shared"] = ForceCast<size_t>(mpff);
		functions["unique"] = ForceCast<size_t>(mpff);
		auto u = std::unique_ptr<TestClass>();
		u->print();
		UnparsedToken temp;
		auto print = new FunctionToken(temp);
		auto add = new FunctionToken(temp);
		auto getInt = new FunctionToken(temp);
		//still need to get string_view, string and vector classes into language
		//later on can do something like this
		//classes["Animal"].functions["bark"] = (size_t)bark_func;
		//it doesnt create the Animal class but provides the function if the class exists
		functions["dlopen"] = ForceCast<size_t>(dlopen);
		MemoryPages mp;
		const char* handle = "11111111111111111111";
		const char* handle2 = "222222222222222222";
		size_t t22 = size_t(-321242194);
		auto p2 = &mp.mem[mp.position];
		int handle_out = (int)p2;
		auto t1 = ForceCast<size_t>(p2);
		classes["ffi"].variables["handle"] = t1;
		int* iii = &test_int;
		//classes["ffi"].variables["handle"] = ForceCast<size_t>(&test_int);
		mp.push_fn(ForceCast<size_t>(handle));
		auto p3 = &mp.mem[mp.position];
		auto t2 = ForceCast<size_t>(p3);
		classes["ffi"].variables["handle2"] = t2;
		mp.push_fn(ForceCast<size_t>(handle2));
		//handle = (size_t)&mp.mem[0];
		int64_t n = 0;
		memcpy(&n, mp.mem, 8);
		classes["ffi"].functions["__create"] = ForceCast<size_t>(&mp.mem[mp.position]);
		//a function is essentially a collection of assembly statements
		//handle = dlopen(str) is the only assembly we need to generate right now
		//mp.push(x86::_start);
		mp.push(0x50); //push rax
		//mp.push(0xA1); mp.push_fn(classes["ffi"].variables["handle"]);    //movabs eax,handle
		mp.push({ 0x48, 0xB8 }); mp.push_fn(classes["ffi"].variables["handle"]);//movabs rax,0x601030
		mp.push({ 0x48, 0x8B, 0x08 });
		mp.push({ 0x48, 0xB8 }); mp.push_fn(classes["ffi"].variables["handle2"]);//movabs rax,0x601030
		mp.push({ 0x48, 0x8B, 0x10 });
		//mp.push({ 0x48, 0x63, 0xF8, 0x48, 0x63, 0xF0, 0x48, 0x63, 0xC8 });
		//mp.push({ 0x48, 0x8B, 0x00 }); //mov rdi, qword ptr [rax]
		//mp.push({ 0x48, 0x89, 0xC7 }); //mov rdi, qword ptr [rax]
		//mp.push({ 0x6A, 0x00 });
		//mp.push(0x57); //push rdi
		//mp.push({ 0x48, 0xBF, 0xC7, 0x19, 0x46, 0x96, 0x02, 0x00, 0x00, 0x00, 0x48, 0xBE, 0x8E, 0x33, 0x8C, 0x2C, 0x05, 0x00, 0x00, 0x00, 0x48, 0xB9, 0xC7, 0x19, 0x44, 0x96, 0x02, 0x00, 0x00, 0x00 });// mp.push_fn(classes["ffi"].variables["handle"]);//, 0x01, 0x00, 0x00, 0x00});
		//mp.push({ 0xBF, 0x66, 0x3A, 0xFA, 0x25 });
		//mp.push({ 0xB8, 0x00, 0x00, 0x00, 0x00 });
		mp.push(0x48); mp.push(0xba); mp.push_fn(functions["dlopen"]);    // movabs rcx, <function_address>
		//mp.push({ 0x31, 0xc0 });
		//mp.push({ 0xBF, 0x63, 0x00, 0x00, 0x00 });
		//mp.push({ 0x48, 0x89, 0x05 }); mp.push_fn(classes["ffi"].variables["handle"]); mp.push({ 0x01, 0x00, 0x00, 0x00 });// mp.push_fn(classes["ffi"].variables["handle"]);
		mp.push({ 0x31, 0x30 });                   // cear eax
		mp.push({ 0xFF, 0xD2 });                   // call rax
		//mp.push({ 0xBa, 0x01, 0x00, 0x00, 0x00 }); //set eax
		mp.push({ 0x58, 0xc3 });                   // pop rax ret

		//mp.push(x86::_end);
		mp.show_memory();
		typedef int(_fastcall *funv2)();
		auto _create = reinterpret_cast<funv2>(ForceCast<uint8_t*>(classes["ffi"].functions["__create"]));
		auto tt1 = _create();
		auto p = &mp.mem[mp.position];
		mp.push({ 0x8b, 0xc1, 0x0f, 0xaf, 0xc0, 0xc3 });
		void* (*mpf)(size_t) = &::operator new;
		functions["new"] = ForceCast<size_t>(mpf);
		functions["square"] = ForceCast<size_t>(p);
		functions["print"] = (size_t)supercooool;
		//somelib = ffi("somelib.dll") //during parse the compiler calls dlopen and returns the handle
		//sqaure(int):int: = somelib.get("square") //parser calls dlsym with the dlopen handle and gets the address of the function and sets that to the function
		functions["ffi"] = (size_t)ffi;
		classes["Lib"].functions["get"] = ForceCast<size_t>(&Lib::get);
		typedef int(_fastcall *funv)(int);
		auto func = reinterpret_cast<funv>(ForceCast<uint8_t*>(functions["square"]));
		int ii = func(8);
		uint8_t*funcAddress = &mp.mem[0];
		//mp.push({ 0xB8, 0x04, 0,0,0, 0xC3}); //move 4 into eax
		//mp.push({ 0x8D, 0x04, 0xbd, 0xC3 });
		//mp.push({ 0x89, 0xF8, 0x0F, 0xAF, 0xC7, 0xC3 });
		//mp.push({ 0x55, 0x48, 0x89, 0xE5, 0xBF, 0x02, 0x00, 0x00, 0x00, 0xE8, (uint8_t)operand, *((uint8_t*)&operand + 1), *((uint8_t*)&operand + 2), *((uint8_t*)&operand + 3),*((uint8_t*)&operand + 4), *((uint8_t*)&operand + 5), *((uint8_t*)&operand + 6), *((uint8_t*)&operand + 7), 0x5D, 0xC3 });
		//mp.push({ 0x8D, 0x04, 0x95, 0x00, 0x00, 0x00, 0x00, 0xC3 }); //square function
		//mp.push({ 0xFF, 0x25, 0xC2, 0x0B, 0x20, 0x00, 0x6A, 0x00, 0xE9, 0xe0, 0xff, 0xff, 0xff }); //operator new
		mp.push(x86::_start);
		//mp.push({ 0xB8, 0x02 });
		//mp.push({ 0x89, 0x7D, 0xFC, 0x8B, 0x7D, 0xFC, 0x83, 0xC7, 0x02, 0x89, 0xF8 });
		mp.push({ 0x89, 0xf8 });
		mp.push(0x48); mp.push(0xb8);
		size_t test1 = (size_t)func;
		//auto operand_start = mp.position;
		mp.push_fn(functions["square"]);    // movabs rax, <function_address>
		mp.push(0xff); mp.push(0xd0);                   // call rax
		mp.push(x86::_end);
		//mp.push({0x2E, 0x66, 0x0F, 0x1F, 0x04, 0x00, 0x0F, 0x1F, 0x04, 0x00});
		//mp.push({0x0F, 0x1F, 0x04, 0x00});
		//ffi is a function that returns a function ptr
		//test = ffi("void test();")
		//test = function pointer 
		//test!
		//turns into
		//


		//v = 1 + 1
		//creates a new variable with the value 2
		//move the value 0x2 into the ptr rbp-0x4
		//mov DWORD PTR [rbp-0x4],0x2
		//{ 0xC7, 0x45, 0xFC, 0x02, 0x00, 0x00, 0x00 } 
		//v = 2 + v
		//v is added to 1 and itself
		//move the ptr rbp-0x4 to eax
		//mov eax,DWORD PTR [rbp-0x4]
		//call add eax and the value 2
		//add eax, 0x2
		//move value at eax to rbp - 0x4
		//mov DWORD PTR[rbp - 0x4], eax
		//{ 0x8B, 0x45, 0xFC, 0x83, 0xC0, 0x02, 0x89, 0x45, 0xFC }
		//generate machine code for a function
		//int test(int a, int b) { return a * b; }
		//mov eax, edi
		//imul eax, esi
		//ret
		//{ 0x8b, 0xc1, 0x0f, 0xaf, 0xc0, 0xc3 }

		/*mp.push({ 0x55, 0x89, 0xE5 });
		mp.push({ 0xE8 });// mp.push(supercooool);
		mp.push({ 0xFC, 0xFF, 0xFF, 0xFF });
		mp.push({ 0x5D, 0xC3 });*/
		//mp.push({ 0x50, 0xC7, 0x44, 0x24, 0x04, 0x00, 0x00, 0x00, 0x00, 0xB8, 0x02, 0x00, 0x00, 0x00, 0x59, 0xC3 });
		//mp.push(x64::_start);
		//mp.push({ 0xe8 }); mp.push(supercooool);    // movabs rax, <function_address>
		//mp.push({ 0xff, 0xd0 });
		//mp.push(x64::_end);
		/*mp.show_memory();
		typedef int(_fastcall *test_int)(int);
		//int(*func)(const char*) = reinterpret_cast<int(*)(const char*)>(mp.mem);
		//func("test");
		auto square = reinterpret_cast<test_int>(&mp.mem[functions["square"]]);
		int iiii = square(2);
		print->name = "print";
		add->name = "add";
		getInt->args.push_back({ "a", "int", "" });
		parsed2->functions["print"] = print;
		std::string out_lua = "";
		auto tt = (ClassToken*)parsed2->root;
		for (auto t : tt->children) {
			t->GetLua(out_lua);
			out_lua += '\n';
		}*/
		return "";
	}
	ParsedResult getCode(const Options flags, const std::string& fn) {
		ParsedResult result;
		std::string source = Helper::readFile(fn);
		std::string out_mc = "";
		if (flags & Options::LUA) {
			result.lua = getLua(source);
		}
		if (flags & Options::CPP) {
			result.lua = getCpp(source);
		}
		if (flags & Options::MACHINE_CODE) {
			result.machine_code = getMachineCode(source);
		}
		return result;
	}
}