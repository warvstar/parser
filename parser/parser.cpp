//convert all to import later
#ifdef USE_SSL
#include "client_wss.hpp"
#include "server_wss.hpp"
using WssServer = SimpleWeb::SocketServer<SimpleWeb::WSS>;
#else
#include "client_ws.hpp"
#include "server_ws.hpp"
using WsServer = SimpleWeb::SocketServer<SimpleWeb::WS>;
using WsClient = SimpleWeb::SocketClient<SimpleWeb::WS>;
#endif
#include "helper.h"
#include "parser.h"
#include "tokens.h"
#include "vm.h"
#include <tuple>
#include <vector>
#include <string_view>
#include <fstream>
#include <chrono>
#include "basic_types.h"
std::unique_ptr<Parsed> Parsed::instance;
std::once_flag Parsed::onceFlag;
Parsed::Parsed() {
}
extern "C" {
#include "ncc.h"
}
#include "bitmask.hpp"
BITMASK_DEFINE(Options);
namespace Parser {
	Token* Parse(std::string_view source, std::vector<std::unique_ptr<Token>>& tokens) {
		std::unique_ptr<ClassToken> root = std::make_unique<ClassToken>();
		std::vector<std::pair<SymbolType, Mixed>> syms = { {SymbolType::Function, "entry1"} , {SymbolType::Colon, ""} };
		std::unique_ptr<FunctionToken> entry_token0 = std::make_unique<FunctionToken>(syms);
		if (tokens.size() == 0) {
			root->name = "root"; root->level = 0;
			tokens.push_back(std::move(root));
			tokens.push_back(std::move(entry_token0));
			FunctionToken* entry_token = (FunctionToken*)tokens[1].get();
			entry_token->parent = tokens.front().get();
			tokens.front().get()->children["entry1"] = entry_token;
			tokens.front().get()->token_type = THE_TOKEN_TYPE::CLASS;
			tokens.front()->level = 0;
		}
		FunctionToken* entry_token = (FunctionToken*)tokens[1].get();
		Helper::splitTokens(tokens, source);
		std::vector<std::string> to_erase;
		auto got = tokens.front().get()->children.find("entry1");

		for (auto p : tokens.front().get()->children) {
			p.second->global = true;
			if (p.second->token_type == THE_TOKEN_TYPE::TOKEN) {
				//variable not declared in function, so put in entry func to get initialized
				entry_token->children[p.first] = p.second;
				to_erase.push_back(p.first);
				p.second->parent = entry_token;
			}
		}
		for (auto e : to_erase)
			tokens.front().get()->children.erase(e);
		return tokens.front().get();
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
	class TestClass {
	public:
		void print() { printf("working"); };
	};
	void print(const char* msg) {
		printf("msg:%s\n", msg);
	}
	struct ffi {
		unsigned long long handle;
	};
	static const char* test22 = "";
	auto timer_start() {
		return std::chrono::high_resolution_clock::now();
	}
	uint64_t timer_end(std::chrono::time_point<std::chrono::high_resolution_clock> start) {
		std::chrono::duration<float, std::milli> duration = std::chrono::high_resolution_clock::now() - start;
		return duration.count();
	}
	struct ffi dlopen(string* lib) {
		std::string lib1 = std::string(lib->data).substr(0, lib->size) + ".dll";
		const char* dllHandle0 = (const char*)LoadLibrary(lib1.data());
		struct ffi f;
		f.handle = (size_t)dllHandle0;
		if (dllHandle0) {
			printf("Loaded library \"%s\" succesfully.\n", lib1.data());
		}
		else {
			printf("Failed to load library \"%s\".\n", lib1.data());
		}
		return f;
	}
	int32_t random(int a, int b) {
		return a + b;
	}
	void print_char(const char* c) {
		printf("%s\n", c);
	}
	void print_ffi(struct ffi* f) {
		printf("%d\n", f->handle);
	}
	//use this for indirect calls
	//void(*func_print_name)(int) = (void*)0x0;
	template<typename T>
	auto RunTest(const Label& l) {
		auto& vm = Parsed::get()->getVM();
		typedef T(_fastcall *print_func)(...);
		auto _print = reinterpret_cast<print_func>(&vm.mp.mem[l.ptr]);
		return _print;
	}
	intptr_t dlsym(struct ffi f, string* c) {
		std::string s = std::string(c->data).substr(0, c->size);
		auto ret = GetProcAddress((HMODULE)f.handle, s.data());
		if (ret) {
			printf("Loaded function \"%s\" succesfully.\n", s.data());
			//save function pointer into memory
			auto& vm = Parsed::get()->getVM();
			//vm.libFunctions[s] = (intptr_t)ret;
			//auto& libName1 = vm.label(libName, (intptr_t)ret);
			//libName1.print(vm);
			//*(intptr_t*)(vm.mp.mem + libName1.ptr) = (intptr_t)random;
			//libName1.position = 8;
			//vm.mp.push({ 0xff, 0x15 });
			//*(uint32_t*)(vm.mp.mem + test.ptr + 2) = operand;
			//fnptr();
			//vm.mp.position += sizeof(intptr_t);
			//typedef void(_fastcall *print_func)(float, float, float, float);
			//auto _print = reinterpret_cast<print_func>(ret);
			//_print(1.0f, 1.0f, 1.0f, 1.0f);
			return (intptr_t)ret;

			//typedef int(_fastcall *funv)(int, int);
			//auto func = reinterpret_cast<funv>(t);
			//auto test = func(1, 1);
		}
		else {
			printf("Failed to load function \"%s\".\n", c->data);
			return 0;
		}
	}
	intptr_t glsym(string* c) {
		//implement this in futurescript
		return 0;
		/*void *p = (void *)wglGetProcAddress(c->data);
		if (p == 0 || (p == (void*)0x1) || (p == (void*)0x2) || (p == (void*)0x3) || (p == (void*)-1))
		{
			HMODULE module = LoadLibraryA("opengl32.dll");
			p = (void *)GetProcAddress(module, c->data);
		}
		size_t t = (size_t)p;
		if (p) {
			printf("Loaded function \"%s\" succesfully.\n", c->data);
			auto& vm = Parsed::get()->getVM();
			/*typedef void(_fastcall *print_func)(float, float, float, float);
			auto _print = reinterpret_cast<print_func>(p);
			_print(1.0f, 1.0f, 1.0f, 1.0f);*/
/*
			return (intptr_t)p;
		}
		else {
			printf("Failed to load function \"%s\".\n", c->data);
			return 0;
		}
		*/
	}
	typedef struct _vector {
		unsigned long long size;
		unsigned long long capacity;
		char* data;
	}vector;
	void test_vector(vector* f) {
		printf("size:%ju\n", f->size);
		printf("cap:%ju\n", f->capacity);
	}
	int test_int = 2;
	std::shared_ptr<WsClient::Connection> conn;
	std::thread client_thread;
	void StartClient() {
		WsClient client("localhost:8080/status");
		client.on_message = [](std::shared_ptr<WsClient::Connection> connection, std::shared_ptr<WsClient::InMessage> in_message) {
			std::cout << "Client: Message received: \"" << in_message->string() << "\"" << std::endl;

			std::cout << "Client: Sending close connection" << std::endl;
			connection->send_close(1000);
		};

		client.on_open = [](std::shared_ptr<WsClient::Connection> connection) {
			std::cout << "Client: Opened connection" << std::endl;
			conn = connection;
		};

		client.on_close = [](std::shared_ptr<WsClient::Connection> /*connection*/, int status, const std::string & /*reason*/) {
			std::cout << "Client: Closed connection with status code " << status << std::endl;
		};

		client.on_error = [](std::shared_ptr<WsClient::Connection> /*connection*/, const SimpleWeb::error_code &ec) {
			std::cout << "Client: Error: " << ec << ", error message: " << ec.message() << std::endl;
		};

		client_thread = std::thread([&client]() {
			client.start();
		});
		client_thread.detach();
	}
	std::atomic<bool> waiting_onserver(false);
	std::string header = "typedef signed char i8;\n\
	typedef short              i16;\n\
	typedef int                i32;\n\
	typedef long long          i64;\n\
	typedef unsigned char      u8;\n\
	typedef unsigned short     u16;\n\
	typedef unsigned int       u32;\n\
	typedef unsigned long long u64;\n\
	typedef const char* native_string;\n\
	typedef u16 f16;\n\
	typedef u32 f32;\n\
	typedef u64 f64;\n\
#define bool _Bool\n\
#define true 1\n\
#define false 0\n\
	struct null {\n\
		int b;\n\
	};";
	bool GetMC(VM& vm, const CPP& source){
		//typedef int(_fastcall * funv)();
		//auto func = reinterpret_cast<funv>(&vm.mp.mem[label.ptr]);
		//uint8_t ret = func();
		std::string to_send = "";
		std::string classes = "";
		std::string exports = "";
		std::string macros = vm.getMacros();
		for (auto c : source.classes) {
			if (c.second.size() != 0) {
				classes += c.second;
				macros += "typedef struct _" + c.first + " " + c.first + ";";
			}
		}
		// = "int main(){return 4;}";
		//e9 e9 02 00 00
		//0xe9 0xee
#if 0
		bool test = true;
		for (auto f : source.functions) {
			//if (!(test = !test))
				//continue;
			std::string ss = f.second.body;
			for (auto c : source.exports) {
				auto split = Helper::split(c.second, ";"); split[2].pop_back();
				auto num = stoull(split[2]);
				num -= stoull(f.second.ptr);
				//Helper::replaceAll(ss, c.first, std::to_string(num));
			}
			ss = header +classes + macros +ss;
			unsigned int l = 0;
			char* ll = new char[255];
			char* cstr = new char[ss.length() + 1];
			strcpy(cstr, ss.c_str());
			Compile(cstr, ss.length(), ll, &l);
			std::string o(ll, l);
			delete[] ll;
			if (o.size() > 0) {
				std::vector<uint8_t> vec(o.begin(), o.end());
				auto& label = vm.label(f.second.name, vec);
				vm.functions[f.second.name].push_back(&label);
				typedef int(_fastcall * print_func)();
				auto _print = reinterpret_cast<print_func>(&vm.mp.mem[label.ptr]);
				if (o.size() == 6) {
					auto i = _print();
					auto i2 = _print();
				}
			}
		}
		return;
#endif
		for (auto c : source.exports)
			exports += c.second;
		to_send += classes + "|||" + exports + "|||" + macros + "|*|";
		for (auto f : source.functions)
			if (f.second.body.size() > 0)
				to_send += f.second.body + "|||" + f.second.name + "|||" + f.second.ptr + "|*|";
		if (conn && to_send.size() > 9 && source.functions.size() > 0) {
			waiting_onserver = true;
			conn->send(to_send);
			return true;
		}
		return false;
	}
	void ReadCin(std::atomic<bool>& run) {
		std::string buffer;
		while (run.load()) {
			std::cin >> buffer;
			if (buffer == "X")
				run.store(false);
		}
	}
	void Run(std::function<void()> func, std::chrono::milliseconds tick) {
		std::atomic<bool> run(true);
		std::thread cinThread(ReadCin, std::ref(run));

		while (run.load()) {
			func();
			std::this_thread::sleep_for(tick);
		}

		run.store(false);
		cinThread.join();
	}
	//later find a way to implement a print function that works similiar to javascript
	void print_num(int num) {
		printf("%d\n", num);
	}
	WsClient* client = nullptr;
	std::atomic<bool> started(false);
	std::atomic<bool> has_error(true);
	bool has_code = false;
	//TODO: if type needs converting, ie from a & to * then do so
	auto alloc(uint64_t size) {
		auto& vm = Parsed::get()->getVM();
		uint64_t val = 0;
		auto t = vm.alloc_var(size, val);
		auto test = *(intptr_t*)&vm.mp.mem[val];
		string* s = (string*)&vm.mp.mem[val];
		string* s1 = (string*)(*(uint64_t*)&vm.mp.mem[val]);
		string* s2 = (string*)&test;
		string* s12 = (string*)test;
		return t;
	}
	std::vector<std::string> split_2_keepall(std::string_view s, const char c) {
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
	std::string GetName(const std::string& str) {
		auto split = split_2_keepall(str, '_');
		return split[2];
	}
	void start() {
		auto parsed2 = Parsed::get();
		parsed2->vm = new VM();
		auto& vm = Parsed::get()->getVM();
		//uint64_t val = 0;
		//string* s = (string*)vm.alloc(24, val);
		//s->size = 1;
		//s->capacity = 2;
		//val = 24;
		//std::memcpy((vm.mp.mem + 4000), &s, sizeof(&s));
		//string* s3 = (string*)(*(intptr_t*)&vm.mp.mem[4000]);
		//s3->size = 1;
		//string* s4 = (string*)&vm.mp.mem[4000];

		//string* s2 = (string*)(*(intptr_t*)&vm.mp.mem[label->second.ptr]);

		//uint64_t* t = (uint64_t*)&vm.mp.mem[0];
		//uint64_t tt = *t;
		//std::cout << tt;
		//vm.mp.show_memory();
		/*//auto v = Helper::calculateCallOperand(funcAddress, currentAddress);
		auto& dl = vm.label("dl", (uint64_t)dl0);
		vm.labels["dl"].ptr = (size_t)dl0;
		dl.def = "extern void dl(int mode);dl;" + std::to_string((uint64_t)dl0) + "\n";;
		dl.type = "void";
		dl.by = By::ptr;*/
		//size_t dlopen2 = (size_t)dlopen;
		uint8_t* dlopen_ptr = (uint8_t*)dlopen;
		uint8_t* alloc_ptr = (uint8_t*)alloc;
		uint8_t* dlsym_ptr = (uint8_t*)dlsym;
		uint8_t* test_vector_ptr = (uint8_t*)test_vector;
		uint8_t* glsym_ptr = (uint8_t*)glsym;
		uint8_t* print_num_ptr = (uint8_t*)print_num;

		vm.labels["_u64_alloc_u64"].ptr = (size_t)alloc;
		vm.labels["_u64_alloc_u64"].name = "_u64_alloc_u64";
		vm.functions["alloc"].push_back(&vm.labels["_u64_alloc_u64"]);
		vm.labels["_u64_alloc_u64"].def = std::string("extern ") + "u64*" + " " + "_u64_alloc_u64" + "();" + "_u64_alloc_u64" + ";" + std::to_string((size_t)alloc_ptr) + "\n";
		vm.labels["_u64_alloc_u64"].SetType({ BasicType::u64, "u64" });

		vm.labels["_ffi_dlopen_str"].ptr = (size_t)dlopen;
		vm.labels["_ffi_dlopen_str"].name = "_ffi_dlopen_str";
		vm.functions["dlopen"].push_back(&vm.labels["_ffi_dlopen_str"]);
		vm.labels["_ffi_dlopen_str"].def = std::string("extern ") + "ffi*" + " " + "_ffi_dlopen_str" + "();" + "_ffi_dlopen_str" + ";" + std::to_string((size_t)dlopen_ptr) + "\n";
		vm.labels["_ffi_dlopen_str"].SetType({ BasicType::var, "ffi" });
		vm.labels["dlsym"].ptr = (size_t)dlsym;
		vm.labels["_void_testvector_vector"].ptr = (size_t)test_vector;
		vm.labels["_void_testvector_vector"].name = "_void_testvector_vector";
		vm.functions["testvector"].push_back(&vm.labels["_void_testvector_vector"]);
		vm.labels["_void_testvector_vector"].def = std::string("extern ") + "void" + " " + "_void_testvector_vector" + "();" + "_void_testvector_vector" + ";" + std::to_string((size_t)test_vector_ptr) + "\n";
		vm.labels["_void_testvector_vector"].SetType({ BasicType::nul, "void" });
		vm.labels["glsym"].ptr = (size_t)glsym;
		//vm.functions["glsym"].push_back(&vm.labels["_u64_glsym_string"]);
		class_def ffi_def;
		ffi_def.name = "ffi";
		ffi_def.variables["handle"] = { BasicType::u64, "u64", uint64_t(0) };
		vm.class_defs["ffi"] = ffi_def;
		vm.reserved_keywords["ffi"] = SymbolType::Type;
		class_def Window_def;
		Window_def.name = "Window";
		Window_def.variables["window"] = { BasicType::u64, "u64", uint64_t(0) };
		Window_def.variables["hDC"] = { BasicType::u64, "u64", uint64_t(0) };
		vm.class_defs["Window"] = Window_def;
		vm.reserved_keywords["Window"] = SymbolType::Type;
		/*class_def string_def;
		string_def.name = "string";
		string_def.variables["u64 size;"] = "0";
		string_def.variables["i8* data;"] = "0";
		vm.class_defs["string"] = string_def;*/
		//any of these should be valid
		//int_vector = [1,2,1,1]
		//int_vector:i64[] = [1,2,1,1]
		//int_vector = i64[1,2,1,1]
		class_def vector_def;
		vector_def.name = "vector";
		vector_def.variables["size"] = { BasicType::u64, "u64", uint64_t(0) };
		vector_def.variables["capacity"] = { BasicType::u64, "u64", uint64_t(0) };
		vector_def.variables["data"] = { BasicType::variadic, "u64*", "0" }; //i8*
		vm.class_defs["vector"] = vector_def;
		vm.reserved_keywords["vector"] = SymbolType::Type;
		class_def string_def;
		string_def.name = "str";
		string_def.variables["data"] = { BasicType::u64, "u64", uint64_t(0) };
		string_def.variables["size"] = { BasicType::u64, "u64", uint64_t(0) };
		string_def.variables["capacity"] = { BasicType::u64, "u64", uint64_t(0) };
		vm.class_defs["str"] = string_def;
		vm.reserved_keywords["str"] = SymbolType::Type;
		class_def variadic_def;
		variadic_def.name = "variadic";
		variadic_def.variables["parameters"] = { BasicType::i8, "i8", "0" };
		variadic_def.variables["size"] = { BasicType::u64, "u64", uint64_t(0) };
		variadic_def.basic = BasicType::variadic;
		vm.class_defs["variadic"] = variadic_def;
		vm.reserved_keywords["variadic"] = SymbolType::Type;
		class_def any_def;
		any_def.name = "any";
		any_def.variables["pointer"] = { BasicType::u64, "u64", SymbolType::Any }; //probably should be u64
		any_def.variables["type"] = { BasicType::u64, "u64", uint64_t(0) };
		vm.class_defs["any"] = any_def;
		vm.reserved_keywords["any"] = SymbolType::Type;



		//auto& vector_size = vm.label("_u64_ffi$get_ffi_string");// , {/*the code*/ }); //has to be called with first argument of Animal


		//string functions
		//constructor
		//deconstructor
		//

		//start with a vector class, string can be built on it, same thing..?




		//printf("%s\n", ffi_def.get_def().data());
		/*auto& test_ideal = vm.labels["test_ideal"];// , {/*the code*/// }); //has to be called with first argument of Animal
		//test_ideal.ptr = (size_t)some;
		/*test_ideal.type = "void";
		test_ideal.by = By::ptr;
		test_ideal.basic = BasicType::u64;
		test_ideal.def = std::string("extern ") + "void" + " " + "test" + "();" + "test" + ";" + std::to_string(test_ideal.ptr) + "\n";

		auto& test = vm.label("test");// , {/*the code*/// }); //has to be called with first argument of Animal
		//test.ptr = (size_t)some;
		/*test.type = "void";
		test.by = By::ptr;
		test.basic = BasicType::u64;
		test.def = std::string("extern ") + "void" + " " + "test" + "();" + "test" + ";" + std::to_string(test.ptr) + "\n";
	//	auto v = Helper::calculateCallOperand(some_ptr, &vm.mp.mem[test.ptr + test.position + 1]);
		//vm.mp.fill(test, { 0xe9,(uint8_t)v,*((uint8_t*)&v + 1),*((uint8_t*)&v + 2),*((uint8_t*)&v + 3) });
//		vm.mp.fill(test, { 0xe8,(uint8_t)v,*((uint8_t*)&v + 1),*((uint8_t*)&v + 2),*((uint8_t*)&v + 3),0xc3 });
		//for (int i = 0, j = 0; i < Main.size(); ++i, ++j)
			//vm.mp.mem[test.ptr] = Main[i];
		//test.position += 5;
		test.print(vm);
		auto offset = vm.mp.mem + test.ptr;
		auto call = (int(*)())offset;*/
		//int i = call();
		//void(*func_print_name)(int) = (void*)0x0;
		//add2 = clib.get("int add2(int, int)")
		//add2(int,int):int = clib.get("add2")
		auto& ffi_get = vm.label("_u64_get_ffi_str");// , {/*the code*/ }); //has to be called with first argument of Animal
		ffi_get.def = std::string("extern ") + "u64" + " " + "_u64_get_ffi_str" + "(ffi*, str*);" + "_u64_get_ffi_str" + ";" + std::to_string(ffi_get.ptr) + "\n";
		ffi_get.SetType({ BasicType::u64, "u64" });
		ffi_get.by = By::ptr;
		ffi_get.basic = BasicType::u64;
		vm.functions["get_ffi"].push_back(&ffi_get);
		vm.mp.fill(ffi_get, { 0x48, 0xB8 });
		std::memcpy((vm.mp.mem + ffi_get.ptr + ffi_get.position), &dlsym_ptr, sizeof dlsym_ptr);
		ffi_get.position += sizeof size_t;
		vm.mp.fill(ffi_get, { 0xff, 0xe0 });

		auto& test_v = vm.label("_void_testv_vector");// , {/*the code*/ }); //has to be called with first argument of Animal
		test_v.def = std::string("extern ") + "void" + " " + "_void_testv_vector" + "(vector*);" + "_void_testv_vector" + ";" + std::to_string(test_v.ptr) + "\n";
		test_v.SetType({ BasicType::nul, "void" });
		test_v.by = By::ptr;
		test_v.basic = BasicType::u64;
		vm.functions["testv"].push_back(&test_v);
		vm.mp.fill(test_v, { 0x48, 0xB8 });
		std::memcpy((vm.mp.mem + test_v.ptr + test_v.position), &test_vector_ptr, sizeof test_vector_ptr);
		test_v.position += sizeof size_t;
		vm.mp.fill(test_v, { 0xff, 0xe0 });


		/*vm.mp.fill(ffi_get, { 0x48, 0xB8 });
 		std::memcpy((vm.mp.mem + ffi_get.ptr + ffi_get.position), &dlopen_ptr, sizeof dlopen_ptr);
		ffi_get.position += sizeof size_t;
		vm.mp.fill(ffi_get, { 0xFF, 0xD0 });
		//ffi_get.call_far(vm, "dlopen");
		ffi_get.ret(vm);*/
		ffi_get.print(vm);
		auto& __glsym = vm.label("_u64_glsym_str");// , {/*the code*/ }); //has to be called with first argument of Animal
		__glsym.def = std::string("extern ") + "u64" + " " + "_u64_glsym_str" + "(str*);" + "_u64_glsym_str" + ";" + std::to_string(__glsym.ptr) + "\n";
		__glsym.SetType({ BasicType::u64, "u64" });
		__glsym.by = By::ptr;
		__glsym.basic = BasicType::u64;
		vm.functions["glsym"].push_back(&__glsym);
		vm.mp.fill(__glsym, { 0x48, 0xB8 });
		std::memcpy((vm.mp.mem + __glsym.ptr + __glsym.position), &glsym_ptr, sizeof glsym_ptr);
		__glsym.position += sizeof size_t;
		vm.mp.fill(__glsym, { 0xff, 0xe0 });
		/*auto& ffi = vm.label("ffi");// , {/*the code*/ //}); //has to be called with first argument of Animal
		/*vm.labels["ffi"].type = "ffi";
		vm.labels["ffi"].by = By::ptr;
		vm.labels["ffi"].basic = BasicType::var;
		vm.labels["ffi"].def = std::string("extern ") + vm.labels["ffi"].GetComplexType() + " " + "ffi" + "(" + "const char*" + ");" + "ffi" + ";" + std::to_string(vm.labels["ffi"].ptr) + "\n";
		//ffi.move(vm, 1, x64::a);
		ffi.call_far(vm, "dlopen");
		ffi.ret(vm);*/
		client = new WsClient("localhost:8080/status");
		client->on_message = [](std::shared_ptr<WsClient::Connection> connection, std::shared_ptr<WsClient::InMessage> in_message) {
			if (in_message->size() > 0) {
				std::string in = in_message->string();
				auto split0 = Helper::splitString(in, "|*|");
				for (auto f : split0) {
					auto split = Helper::splitString(f, "|||");
					if (split.size() > 2) {
						//std::cout << "Assembly received for function " << split[2] << ": " << std::endl << "\"" << split[0] << "\"" << std::endl;
						//std::cout << "Code received: " << std::endl;
						//std::cout << split[1] << std::endl << std::endl;
						//std::cout << "Code size: " << split[1].size() << " bytes." << std::endl;
						std::vector<uint8_t> vec(split[1].begin(), split[1].end());
						//printf("Added function (%s) to virtual machine", split[2].data());
						//auto entry = Parsed::get()->vm->label("entry1", vec);
						auto& vm = Parsed::get()->getVM();
						auto name = GetName(split[2]);
						if (name.back() == '$') {
							auto name_old = name;
							name.pop_back();
							Helper::replaceAll(split[2], name_old, name);
							split[2] += "$";
						}
						auto& label = vm.label(split[2], vec);
						label.debug = split[0];
						vm.functions[name].push_back(&label);
						//if (add2 != vm.labels.end()) {
						//	printf("After code:\n");
						//	add2->second.print(vm);
						//}
						/*auto label3 = vm.label("entry2");
						vm.move(1, x64::a); //1 in rcx
						vm.ret();
						vm.mp.fill(entry, { 0x48, 0x83, 0xEC, 0x28 });
						entry.call_near(vm, "ffi");
						entry.call_near(vm, "ffi");
						vm.mp.fill(entry, { 0xB0, 0x02, 0x48, 0x83, 0xC4, 0x28, 0xC3 });*/
						//entry.print(Parsed::get()->getVM());
						waiting_onserver = false;
						//vm.call("ffi");
						//vm.move(1, x64::a); //1 in rcx
						//vm.ret();
						//vm.print();
						//label.print(vm);
						//vm.mp.show_memory();
						//uint8_t ret = vm.Run<uint8_t>(split[2])();
					}
				}
			}

			//std::cout << "Client: Sending close connection" << std::endl;
			//connection->send_close(1000);
		};

		client->on_open = [&](std::shared_ptr<WsClient::Connection> connection) {
			started = true;
			std::cout << "Client: Opened connection" << std::endl;
			conn = connection;
		};

		client->on_close = [](std::shared_ptr<WsClient::Connection> /*connection*/, int status, const std::string & /*reason*/) {
			std::cout << "Client: Closed connection with status code " << status << std::endl;
		};

		client->on_error = [&](std::shared_ptr<WsClient::Connection> /*connection*/, const SimpleWeb::error_code &ec) {
			//std::cout << "Client: Error: " << ec << ", error message: " << ec.message() << std::endl;
			has_error = true;
		};

		client_thread = std::thread([&]() {
			while (has_error) {
				has_error = false;
				client->start();
			}
		});
		client_thread.detach();
		while (!started)
			std::this_thread::sleep_for(std::chrono::milliseconds(5));
		getMachineCode(vm, "ffi(s:str):\n\ dlopen(s)\nallocc(u:u64):\n\ alloc(u)\n", 1); //ffi(lib:str):\n\ dlopen(lib)\n"
		//getMachineCode("clib = ffi(\"crtdll\")\n", 1); //ffi(lib:str):\n\ dlopen(lib)\n"
		//getMachineCode("print(num:u8):u8 = clib.get(\"printf\")\n", 1); //ffi(lib:str):\n\ dlopen(lib)\n"
		//getMachineCode("entry1:\n\ f = \"test\"\n"); //ffi(lib:str):\n\ dlopen(lib)\n"
		//getMachineCode("entry1:\n\ ff = ffi(\"Dll2\")\n add2 = ff.get(\"AddI\")\n"); //ffi(lib:str):\n\ dlopen(lib)\n"
		//getMachineCode("entry1:\n\ ff = ffi(\"Dll2\")\nentry1()"); //ffi(lib:str):\n\ dlopen(lib)\n"
		//getMachineCode("entry1:\n\ add2 = ff.get(\"AddI\")\nentry1()\n"); //ffi(lib:str):\n\ dlopen(lib)\n"
		
		//getMachineCode("ffi_get:\n\ dlsym()\n", 1); //ffi(lib:str):\n\ dlopen(lib)\n"
	}
	void end() {
		//client_thread1.join();
	}
	std::vector<std::string> que;
	void queueMachineCode(VM& vm, std::string_view source, bool _abs) {
		que.push_back(source.data());
	}
	bool getMachineCode(VM& vm, std::string_view source, bool _abs) {
		if (waiting_onserver) {
			//already processing code
			return false;
		}
		//vm.labels["rand"].ptr = (size_t)print;
		//vm.labels["print"].ptr = (size_t)print;
		//vm.labels["print"].def = std::string("extern ") + "struct null" + " " + "print" + "(" + "const char*, ..." + ");" + "print" + ";" + std::to_string(vm.labels["print"].ptr) + "\n";
		//int ret = vm.Run<int>("ffi")();
		//vm.label("ffi:get");// , {/*the code*/ }); //has to be called with first argument of Animal
		//vm.labels["ffi:get"].def = std::string("extern ") + "void*" + " " + "ffi_get" + "(" + "struct ffi, const char*" + ");" + "ffi_get" + ";" + std::to_string(vm.labels["ffi:get"].ptr) + "\n";
		//vm.call("dlsym"); //dlsym(libname);
		//vm.ret();
		std::string out_cpp = vm.getMacros();
		//parsed2->tokens.clear();
	label_restart:
		CPP block;
		std::vector<std::unique_ptr<Token>> tokens;
		auto tt = (ClassToken*)Parse(source, tokens);
		int restart = 1;
		while (restart--) {
			for (auto p : tt->children) {
				if (p.second->needs_compile) {
					auto status = p.second->PreProcess(vm);
					if (status == RESTART) {
						restart++;
						tokens.clear();
						tt = (ClassToken*)Parse(source, tokens);
						break;
					}
					else if (status == EXIT) {
						return false;
					}
				}

			}
		}
		restart = 1;
		while (restart--) {
			for (auto t : tt->children) {
				if (t.second->needs_compile) {
					t.second->_abs = _abs;
					auto start_size = tt->children.size();
					auto status = t.second->GetCPP(vm);
					if (start_size != tt->children.size() || status == RESTART) {
						restart++;
						if (t.second->out_cpp.functions.size() > 0)
							goto label_waiting;
						else
							goto label_restart;
					}
					t.second->needs_compile = false;
					if (t.second->broken)
						return false;

				}
				//out_cpp += block += '\n';
			}
		}
	label_waiting:
		for (auto& p : tokens)
			block += p->out_cpp;
		bool worked = GetMC(vm, block);
		while (waiting_onserver) {
			//wait on the server to respond
		}
		if (restart == 1) {
			goto label_restart;
		}
		while (que.size() > 0) {
			auto que2 = que;
			que.clear();
			for (auto q : que2)
				getMachineCode(vm, q);
		}
		return worked;
	}
	bool preprocess(VM& vm, std::string_view source, std::vector<std::unique_ptr<Token>>& tokens, bool _abs) {
		auto parsed2 = Parsed::get();
		tokens.clear();
		auto root = (ClassToken*)Parse(source, tokens);
	label_restart:
		int restart = 1;
		while (restart--) {
			for (auto p : root->children) {
				if (p.second->needs_compile) {
					auto status = p.second->PreProcess(vm);
					if (status == RESTART) {
						restart++;
						//tokens.clear();
						//root = (ClassToken*)Parse(source, tokens);
						break;
					}
					else if (status == EXIT) {
						return false;
					}
				}

			}
		}
		if (restart == 1) {
			goto label_restart;
		}
		return true;
	}
	bool compile(VM& vm, std::string_view source, bool _abs) {
		std::vector<std::unique_ptr<Token>> tokens;
		preprocess(vm, source, tokens);
		return compile(vm, tokens);
	}
	bool compile(VM& vm, std::vector<std::unique_ptr<Token>>& tokens, bool _abs) {
		auto out = tocpp(vm, tokens, _abs);
		GetMC(vm, out);
		while (waiting_onserver) {
			//wait on the server to respond
		}
		if (out.functions.size() > 0)
			return true;
		return false;
	}
	CPP tocpp(VM& vm, std::string_view source, bool _abs) {
		std::vector<std::unique_ptr<Token>> tokens;
		preprocess(vm, source, tokens, _abs);
		return tocpp(vm, tokens);
	}
	CPP tocpp(VM& vm, std::vector<std::unique_ptr<Token>>& tokens, bool _abs) {
		CPP out;
		auto root = tokens.front().get();
	label_restart:
		int restart = 1;
		while (restart--) {
			for (auto t : root->children) {
				if (t.second->needs_compile) {
					t.second->_abs = _abs;
					auto start_size = root->children.size();
					auto status = t.second->GetCPP(vm);
					if (start_size != root->children.size() || status == RESTART) {
						restart++;
						if (t.second->out_cpp.functions.size() > 0)
							goto label_waiting;
						else
							goto label_restart;
					}
					t.second->needs_compile = false;
					if (t.second->broken)
						return out;
				}
				//out_cpp += block += '\n';
			}
		}
	label_waiting:
		while (que.size() > 0) {
			auto que2 = que;
			que.clear();
			for (auto q : que2)
				out += tocpp(vm, q);
		}
		for (auto& p : tokens)
			out += p->out_cpp;
		if (restart == 1) {
			goto label_restart;
		}

		return out;
	}
	Variant execMachineCode(VM& vm, std::string_view source) {
		CPP out_cpp;
		has_code = "";
		std::vector<std::unique_ptr<Token>> tokens;
		auto root = (ClassToken*)Parse(source, tokens);
		for (auto p : root->children) {
			p.second->PreProcess(vm);
		}
		for (auto t : root->children) {
			t.second->GetCPP(vm);
			//block.body += '\n';
		}
		for (auto& t : tokens)
			out_cpp += t->out_cpp;
		GetMC(vm, out_cpp);
		while (1) {
			if (has_code) {

				return Variant();
			}
		}
		return Variant();
	}
	bool getMachineCode(VM& vm, std::vector<Token*>& tokens) {
		if (waiting_onserver) {
			//already processing code
			return false;
		}
		CPP block;
		for (auto p : tokens) {
			if (p->needs_compile) {
				auto status = p->PreProcess(vm);
			}

		}
		for (auto p : tokens) {
			if (p->needs_compile) {
				auto status = p->GetCPP(vm);
			}

		}
		for (auto p : tokens)
			block += p->out_cpp;
		GetMC(vm, block);
		while (waiting_onserver) {
			//wait on the server to respond
		}
		return true;
	}
}