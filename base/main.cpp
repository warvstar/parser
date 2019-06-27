#include <iostream>
#include "parser.h"
#include <Windows.h>
#include <atomic>
#include "vm.h"
#include "helper.h"
#include <future>
#include "basic_types.h"
#include <filesystem>
template <typename R>
R callFunc_fast(void* funcaddr) {
	typedef R(*Function)();
	return ((Function)funcaddr)();
}
using namespace std::chrono_literals;
void timer_start(std::function<void(void)> func, std::chrono::milliseconds interval) {
	std::thread([func, interval]() {
			while (true) {
				auto x = std::chrono::steady_clock::now() + interval;
				func();
				std::this_thread::sleep_until(x);
			}
		}).detach();
}
int main() {
	Parser::start();
	auto p = Parsed::get();
	auto& vm = *p->vm;
	//Load vm here
	vm.load();
	//run async loop here to auto update vm
	timer_start([&]() {vm.append();}, 100ms);
	auto entry = vm.labels.find("__entry1_");
	callFunc_fast<void>((void*)& vm.mp.mem[entry->second.ptr]);
	return 1;
}