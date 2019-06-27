#include <iostream>
#include "parser.h"
#include <Windows.h>
#include <atomic>
#include "vm.h"
#include "helper.h"
#include <future>
#include "basic_types.h"
#include <filesystem>
std::string func = "";
bool in_function = false;
#undef max
bool timing = false;
template <typename R>
R callFunc(void* funcaddr) {
	typedef R(*Function)();
	Function fnptr = (Function)funcaddr;
	auto start = std::chrono::high_resolution_clock::now();
	auto t = fnptr();
	std::chrono::duration<float, std::milli> duration = std::chrono::high_resolution_clock::now() - start;
	if (timing) {
		printf("\ntook %f ms\n", duration.count());
		timing = false;
	}
	return t;
}
template <typename R>
R callFunc_fast(void* funcaddr) {
	typedef R(*Function)();
	return ((Function)funcaddr)();
}
void callFunc_noRet(void* funcaddr) {
	typedef void(*Function)();
	Function fnptr = (Function)funcaddr;
	auto start = std::chrono::high_resolution_clock::now();
	fnptr();
	std::chrono::duration<float, std::milli> duration = std::chrono::high_resolution_clock::now() - start;
	if (timing) {
		printf("\ntook %f ms\n", duration.count());
		timing = false;
	}
}
template <typename R, typename... Args>
R callFunctionUsingMemAddress(void* funcaddr, Args... args) {
	typedef R(*Function)(Args...);
	Function fnptr = (Function)funcaddr;
	return fnptr(args...);
}
template <typename R, typename... Arg>
R do_call(void* funcaddr, std::vector<Variant> args) {
	if (args.size() == 0)
		return callFunctionUsingMemAddress<R>(funcaddr);
	else if (args.size() == 1)
		return callFunctionUsingMemAddress<R>(funcaddr, std::visit([](auto&& arg) {return arg; }, args[0]));
	
	return callFunctionUsingMemAddress<R>(funcaddr);

}

std::vector<std::unique_ptr<Token>> Module_preprocess(VM& vm, const std::string& module) {
	std::vector<std::unique_ptr<Token>> tokens;
	Parser::preprocess(vm, module, tokens);
	return tokens;
}
void Module_getMC(VM& vm, std::vector<std::unique_ptr<Token>>& tokens) {
	Parser::compile(vm, tokens);
}
void LoadModule(VM& vm, const std::string& module) {
	auto src = Helper::readFile(module);
	auto tokens = Module_preprocess(vm, src);
	Module_getMC(vm, tokens);
}
void LoadModules(VM& vm, const std::vector<std::string>& modules) {
	std::vector<std::vector<std::unique_ptr<Token>>> precompiled;
	for (auto m : modules) {
		auto src = Helper::readFile(m);
		if (src.size() != 0)
			precompiled.push_back(Module_preprocess(vm, src));
		else
			printf("Module (%s) non existent or empty.\n", m.data());
	}
	//int i = 0;
	for (auto& m : precompiled) {
		Module_getMC(vm, m);
		//printf("Loaded module:%s\n", modules[]);
	}
	auto entry = vm.labels.find("__entry1_");
	callFunc_fast<void>((void*)& vm.mp.mem[entry->second.ptr]);
}
void LoadModules(VM& vm, const std::string& module_dir) {
	std::vector<std::string> modules;
	for (const auto& entry : std::filesystem::directory_iterator(module_dir))
		modules.push_back(entry.path().string());
	LoadModules(vm, modules);
}
#include <zstd.h>
void ReadCin(std::atomic<bool>& run) {
	std::string buffer;
	auto p = Parsed::get();
	auto& vm = *p->vm;
	//auto start = std::chrono::high_resolution_clock::now();
	//auto end = std::chrono::high_resolution_clock::now();
	//bool timing = false;
	while (run.load()) {
		printf(">");
		bool is_function = false;
		bool defining_func = false;
		std::getline(std::cin, buffer);
		//start = std::chrono::high_resolution_clock::now();

		/*int has_left_paren = buffer.find('('); //TOD: hack, fix this
		if (has_left_paren > 0) {
			int has_equals = buffer.find('='); //TOD: hack, fix this
			if (has_equals > has_left_paren) {
				auto script = Parser::getCodeString(Options::MACHINE_CODE, buffer);
				continue;
			}
		}*/
		//std::replace(buffer.begin(), buffer.end(), '\\', '\\');
		Helper::replaceAll(buffer, "\\n", "\n");

		if (buffer == "save()") {

			continue;
		}
		else if (buffer == "tt()") {
			LoadModules(vm, "./Modules/");
			continue;
		}
		else if (buffer.substr(0, 5) == "load(") {
			buffer.pop_back();
			buffer = buffer.substr(5); 
			LoadModules(vm, buffer);
			//LoadModule(vm, "./Modules/Animal.fe");
			continue;
		}
		else if (buffer == "load()") {

			continue;
		}
		else if (buffer.substr(0, 5) == "time(") {
			buffer.pop_back();
			buffer = buffer.substr(5);
			timing = true;
			//exit(1);
		}
		else if (buffer == "aot()") {
			//convert all to c and save as exe
			continue;
		}
		else if (buffer.substr(0, 12) == "SetFontSize(") {
			buffer.pop_back();
			buffer = buffer.substr(12);
			CONSOLE_FONT_INFOEX cfi;
			cfi.cbSize = sizeof(cfi);
			cfi.nFont = 0;
			cfi.dwFontSize.X = 0;                   // Width of each character in the font
			cfi.dwFontSize.Y = std::stoi(buffer);                  // Height
			cfi.FontFamily = FF_DONTCARE;
			cfi.FontWeight = FW_NORMAL;
			std::wcscpy(cfi.FaceName, L"Consolas"); // Choose your font
			SetCurrentConsoleFontEx(GetStdHandle(STD_OUTPUT_HANDLE), FALSE, &cfi);
			continue;
		}
		else if (buffer.substr(0, 8) == "RunTest(") {
			buffer.pop_back();
			buffer = buffer.substr(8);
			auto file = Helper::readFile(buffer);
			if (file != "") {
				printf("Running test (%s).\n", buffer.data());
				auto ret = Parser::getMachineCode(vm, file);
			}
			else {
				printf("Unknown test (%s).\n", buffer.data());
			}
			continue;
		}
		if (buffer == "getMem()") {
			auto got = vm.labels.find("__entry1_");
			if (got != vm.labels.end())
				got->second.position = 0;
			vm.printAllLabels();
			auto str = vm.getAllLabelsMachineCode();
			std::string str2 = "";
			for (auto val : str)
				str2 += Helper::int_to_hex(UCHAR(val)) + " ";
			printf("Memory:%s\n", str2.data());
			printf("Bytes used:%d\n", str.size());
			size_t const cBuffSize = ZSTD_compressBound(str.size());
			uint8_t* const cBuff = new uint8_t[cBuffSize];
			size_t const cSize = ZSTD_compress(cBuff, cBuffSize, str.data(), str.size(), 1);
			std::vector<uint8_t> v(cBuff, cBuff + cSize);
			str = "";
			for (auto val : v)
				str += Helper::int_to_hex(val) + " ";
			delete cBuff;
			printf("Memory after compression:%s\n", str.data());
			printf("Bytes used after compression:%d\n", v.size());
			continue;
		}
		if (buffer == "test") {
			auto got = vm.labels.find("add2");
			got->second.print(vm);
			typedef int(_fastcall *funv)(int, int);
			auto func = reinterpret_cast<funv>(&vm.mp.mem[got->second.ptr]);
			uint8_t ret = func(1,1);
			continue;
		}
		if (buffer.size() == 0) {
			if (func.size() > 0) {
				auto script = Parser::getMachineCode(vm, func);
				func = "";
				in_function = false;
			}
			continue;
		}
		if (buffer.back() == ':' || buffer.substr(0, 6) == "class ")
			in_function = true;
		else if (buffer.back() == '!' || buffer.back() == ')' && !in_function) {
			is_function = true;
			Label* label = nullptr;
			std::vector<Variant> args = {}; // not even needed
			if (buffer.back() != '!') { //else exec directly, no need to compile string
				//std::pair<std::string, std::string> func = GetFunctionNameAndArgs(buffer);
				bool is_valid_func_call = true;
				if (is_valid_func_call) {
					//std::string start = "entry1:\n\ " + func.first + "(" + func.second + ")";
					//std::string start = "entry1:\n " + buffer + "\n";
					std::string start = buffer + "\n";
					auto ret = Parser::getMachineCode(vm, start);
					if (ret == 0)
						continue;
					auto got = vm.labels.find("__entry1_");
					if (got != vm.labels.end()) {
						//got->second.print(vm);
					if (got->second.basic == BasicType::u8) {
						auto v = (void*)& vm.mp.mem[got->second.ptr];
						uint8_t c = 0;
						//auto start = std::chrono::high_resolution_clock::now();
						//for (int i = 0; i < 1000000; ++i)
							c += callFunc_fast<uint8_t>(v);
						//std::chrono::duration<float, std::milli> duration = std::chrono::high_resolution_clock::now() - start;
						//printf("\ntook %f ms\n", duration.count());
						std::cout << (uint16_t)c << std::endl;
					}
					else if (got->second.basic == BasicType::u16) {
						auto c = callFunc<uint16_t>((void*)& vm.mp.mem[got->second.ptr]);
						std::cout << (uint16_t)c << std::endl;
					}
					else if (got->second.basic == BasicType::i8) {
						auto c = callFunc<int8_t>((void*)&vm.mp.mem[got->second.ptr]);
						std::cout << (uint16_t)c << std::endl;
					}
					else if (got->second.basic == BasicType::u64) {
						auto c = callFunc<uint64_t>((void*)&vm.mp.mem[got->second.ptr]);
						std::cout << c << std::endl;
					}
					else if (got->second.GetType().type == BasicType::str) {
						auto c = callFunc<string*>((void*)&vm.mp.mem[got->second.ptr]);
						//std::cout << c << std::endl;
						/*if (c) {
							printf("size:%ju ", c->size);
							printf("cap:%ju\n", c->capacity);
							if (c->data)
								std::cout << (const char*)c->data << std::endl;
						}*/
					}
					/*else if (got->second.GetType() == "any") {
						auto c = callFunc<any*>((void*)&vm.mp.mem[got->second.ptr]);
						//std::cout << c << std::endl;
						if (c) {
							printf("pointer:%ju\n", c->pointer);
							if (c->type.data)
								std::cout << (const char*)c->type.data << std::endl;
						}
					}*/
					else {
						if (got->second.GetType().type == BasicType::nul) {
							callFunc_noRet((void*)&vm.mp.mem[got->second.ptr]); 
							//std::thread{ callFunc_noRet, (void*)&vm.mp.mem[got->second.ptr] }.detach();
							//std::async(std::launch::async, [&] { callFunc_noRet((void*)&vm.mp.mem[got->second.ptr]); });
							
							continue;
						}
						callFunc_noRet((void*)&vm.mp.mem[got->second.ptr]);
						//std::cout << c << std::endl;
					}
					}
					else {
						printf("Label(%s) does not exist. code:166\n", buffer.data());
					}
				}
			}
			else {
				buffer.pop_back();
				auto got = vm.labels.find(buffer);
				if (got != vm.labels.end()) {
					label = &got->second;
				}
				if (label) {
					if (label->basic == BasicType::i32) {
						//auto c = do_call<int32_t>((void*)&vm.mp.mem[label->ptr], args);
						//std::cout << c << std::endl;
					}
					else if (label->basic == BasicType::u8) {
						//auto c = do_call<uint8_t>((void*)&vm.mp.mem[label->ptr], args);
						//std::cout << (uint16_t)c << std::endl;
					}
				}
				else {
					printf("Label(%s) does not exist. code:187\n", buffer.data());
				}
			}
			//Run2<int>((void*)&vm.mp.mem[vm.labels[buffer].ptr], args);
			//unpack_vector<1>(args);
			//auto ret = Run4(_print, args);
/*			std::variant<uint8_t, void*, const char*> ret = */
			/*if (std::holds_alternative<uint8_t>(ret)) {
				auto _ret = std::get<uint8_t>(ret);
				std::cout << _ret << std::endl;
			}
			else if (std::holds_alternative<const char*>(ret)) {
				auto _ret = std::get<const char*>(ret);
				std::cout << _ret << std::endl;
			}
			else if (std::holds_alternative<void*>(ret)) {

			}*/
			continue;
		}
		else {
			if (!in_function) {
				//get var data
				int has_equals = buffer.find('='); //TOD: hack, fix this
				if (has_equals > -1) {
					//compile this statement
					std::string start = "";
					std::string tmp = "enum";
					if (buffer.substr(0, tmp.size()) != tmp)
						//start = "entry1:\n " + buffer + "\n";
						start = buffer + "\n";
					else
						start = buffer + "\n";
					auto ret = Parser::getMachineCode(vm, start);
					if (!ret)
						continue;
					//continue;
					auto got = vm.labels.find("__entry1_");
					if (got != vm.labels.end()) {
						callFunc_fast<void>((void*)& vm.mp.mem[got->second.ptr]);
						/*if (got->second.basic == BasicType::u8) {
							auto c = callFunc<uint8_t>((void*)&vm.mp.mem[got->second.ptr]);
							std::cout << (uint16_t)c << std::endl;
						}
						else if (got->second.basic == BasicType::i32) {
							if (got->second.GetType().type == BasicType::nul) {
								callFunc_noRet((void*)&vm.mp.mem[got->second.ptr]);
								continue;
							}
							auto c = callFunc<int32_t>((void*)&vm.mp.mem[got->second.ptr]);
							std::cout << c << std::endl;
						}
						else {
							callFunc_noRet((void*)&vm.mp.mem[got->second.ptr]);
						}*/
					}
					else {
						printf("Label(%s) does not exist. code:233\n", buffer.data());
					}
				}
				else {
					if (buffer[0] == '#') {
						int end_exp = buffer.find_first_of(" ");
						std::string var = buffer.substr(1, end_exp - 1);
						std::string val = buffer.substr(var.size() + 1);
						end_exp = val.find_first_not_of(" ");
						val = val.substr(end_exp);

						vm.addMacro(var, val);
						continue;
					}
					int found_period = buffer.find('.');
					std::string method_sig = "";
					std::string right = "";
					if (found_period > -1) {
						auto split = Helper::split_1(buffer, '.');
						buffer = split[0];
						right = split[1];
					}
					auto label = vm.labels.find(buffer);
					//auto& label = vm.label(buffer);
					if (label != vm.labels.end()) {
						if (is_function) {
							label->second.print(vm);
						}
						else {
							label->second.print(vm);
							//is variable, print variable
							std::cout << label->second.GetType().type_name << std::endl;
							if (label->second.GetType().type == BasicType::u8)
								std::cout << (int)*(uint8_t*)&vm.mp.mem[label->second.ptr] << std::endl;
							else if (label->second.GetType().type == BasicType::u16) {
								std::cout << *(uint16_t*)&vm.mp.mem[label->second.ptr] << std::endl;
							}
							else if (label->second.GetType().type == BasicType::u32) {
								std::cout << *(uint32_t*)&vm.mp.mem[label->second.ptr] << std::endl;
							}
							else if (label->second.GetType().type == BasicType::u64) {
								std::cout << *(uint64_t*)&vm.mp.mem[label->second.ptr] << std::endl;
							}
							else if (label->second.GetType().type == BasicType::i8)
								std::cout << (int)*(int8_t*)&vm.mp.mem[label->second.ptr] << std::endl;
							else if (label->second.GetType().type == BasicType::i16) {
								std::cout << *(int16_t*)&vm.mp.mem[label->second.ptr] << std::endl;
							}
							else if (label->second.GetType().type == BasicType::i32) {
								std::cout << *(int32_t*)&vm.mp.mem[label->second.ptr] << std::endl;
							}
							else if (label->second.GetType().type == BasicType::i64) {
								std::cout << *(int64_t*)&vm.mp.mem[label->second.ptr] << std::endl;
							}
							else if (label->second.GetType().type == BasicType::f32) {
								std::cout << *(float*)&vm.mp.mem[label->second.ptr] << std::endl;
							}
							else if (label->second.GetType().type == BasicType::f64) {
								std::cout << *(double*)&vm.mp.mem[label->second.ptr] << std::endl;
							}
							else if (label->second.GetType().type == BasicType::str) {
								int a = sizeof(string);
								auto test = *(intptr_t*)&vm.mp.mem[label->second.ptr];
								string* s = (string*)(*(intptr_t*)&vm.mp.mem[label->second.ptr]);
								if (s) {
									printf("size:%ju\n", s->size);
									printf("cap:%ju\n", s->capacity);
									if (s->data)
										std::cout << (const char*)s->data << std::endl;
								}
							}
							else if (label->second.GetType().type == BasicType::var) {
								if (label->second.GetType().type_name == "any") {
									int a = sizeof(any);
									auto test = *(intptr_t*)&vm.mp.mem[label->second.ptr];
									any* s2 = (any*)&vm.mp.mem[label->second.ptr];
									any* s = (any*)(*(intptr_t*)&vm.mp.mem[label->second.ptr]);
									for (int i = 0; i < 24; ++i)
										printf("%d", int(vm.mp.mem[i + label->second.ptr]) > 0);
									printf("\n");
									for (int i = 24; i < 50; ++i)
										printf("%d", int(vm.mp.mem[i + label->second.ptr]) > 0);
									if (s) {
										printf("pointer:%ju\n", s->pointer);
										if (s->type.data)
											std::cout << (const char*)s->type.data << std::endl;
									}
								}
								else if (label->second.GetType().type_name == "vector") {
									typedef struct _vector {
										unsigned long long size;
										unsigned long long capacity;
										char* data;
									}vector;
									vector* f = (vector*)(*(uint64_t*)&vm.mp.mem[label->second.ptr]);
									f->size = 21;
									vector* ff = (vector*)&vm.mp.mem[label->second.ptr + 1];
									vector* fff = (vector*)&vm.mp.mem[label->second.ptr] + 1;
									std::cout << *(uint64_t*)&vm.mp.mem[label->second.ptr] << std::endl;
								}
							}
						}
					}
					else {
						auto func = vm.class_defs.find(buffer);
						if (func != vm.class_defs.end()) {
							if (func->second.extends.size() > 0)
								printf("class %s extends %s\n", buffer.data(), func->second.extends.data());
							else
								printf("class %s\n", buffer.data());
							for (auto f : func->second.get_variables(vm)) {
								std::string out = "";
								out += f.first;
								if (f.second.type == BasicType::u64)
									out += ":u64";
								else if (f.second.type == BasicType::i64)
									out += ":i64";
								else if (f.second.type == BasicType::u8)
									out += ":u8";
								else if (f.second.type == BasicType::str)
									out += ":string";
								printf(" %s\n", out.data());
							}
							printf("\n");
							for (auto f : func->second.functions) {
								std::string out = "";
								out += f.first;
								printf(" %s\n", out.data());
							}
						}
						else {
							auto func = vm.functions.find(buffer);
							if (func != vm.functions.end()) {
								//print all functions
								printf("Functions:\n");
								for (auto f : func->second)
									printf("  %s\n", f->name.data());
							}
							else
								printf("Label(%s) does not exist. code:287\n", buffer.data());
						}
					}
				}
				
				continue;
			}
		}
		buffer += '\n';
		if (in_function) {
			func += buffer;
		}
		else {
			if (buffer.front() != ' ') {
				if (func.size() > 0) {
					auto script = Parser::getMachineCode(vm, func);
					func = "";
					in_function = false;
					continue;
				}
			}
			if (buffer.size() > 0)
				auto script = Parser::getMachineCode(vm, buffer);
		}

		//if (buffer == "X")
			//run.store(false);
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
void ReplLoop() {

}
using namespace std::chrono_literals;
int main() {
	//sol::state lua;
	//lua.open_libraries(sol::lib::base, sol::lib::ffi, sol::lib::jit, sol::lib::package);
	//auto script = Parser::getLua("../parser/gl.fe");
	Parser::start();
	//auto script = Parser::getCode(Options::MACHINE_CODE, "../parser/test_0.fe");
	//auto script = Parser::getCodeString(Options::MACHINE_CODE, "entry1:\n\ 1 + 1\n");

	Run(ReplLoop, 50ms);
	//import gl
	//in lua would generate
	//require('gl')
	//in c++ would generate, based on the exports the parser found
	//auto handle = dlopen("lib");
	//auto func = dlsym(handle, "func");


	/*try {
		lua.safe_script(script);
	}
	catch (std::exception &e) {
		luaL_error(lua, e.what());
	}*/
}
// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
