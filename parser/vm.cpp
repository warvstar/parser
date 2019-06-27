#include "vm.h"
#include "helper.h"
#include "parser.h"
#include "fmt/format.h"
#include "basic_types.h"
using namespace fmt;

VM::VM() {
	free_memory_positions.push_back({ 0, mp.mem_size/2 });
	free_memory_positions_vars.push_back({ mp.mem_size/2, mp.mem_size});
}

void VM::save() {
	auto data = this->labels.da
}
void VM::append() {

}
void VM::load() {

}
intptr_t VM::alloc(uint64_t size, uint64_t& pos) {
	bool found = false;
	for (auto& p : free_memory_positions) {
		if ((p.end - p.start) >= size) {
			if ((p.end - p.start) == size)
				p.end = 0;
			auto old = p.start;
			p.start += size;
			pos = old;
			found = true;
			break;
		}
	}
	free_memory_positions.erase(std::remove_if(free_memory_positions.begin(), free_memory_positions.end(), [](mem_info v) { return v.end == 0; }), free_memory_positions.end());
	if (found) {
		//memcpy should not be needed here, should be faster
		//std::memcpy((mp.mem + pos), &mp.mem[pos], sizeof(&mp.mem[pos]));
		return (intptr_t)& mp.mem[pos];
	}
	else {
		pos = 0;
		printf("Out of memory.\n");
		return -1;
	}
}
intptr_t VM::alloc_var(uint64_t size, uint64_t& pos) {
	bool found = false;
	for (auto& p : free_memory_positions_vars) {
		if ((p.end - p.start) >= size) {
			if ((p.end - p.start) == size)
				p.end = 0;
			auto old = p.start;
			p.start += size;
			pos = old;
			found = true;
			break;
		}
	}
	free_memory_positions_vars.erase(std::remove_if(free_memory_positions_vars.begin(), free_memory_positions_vars.end(), [](mem_info v) { return v.end == 0; }), free_memory_positions_vars.end());
	if (found) {
		//memcpy should not be needed here, should be faster
		//std::memcpy((mp.mem + pos), &mp.mem[pos], sizeof(&mp.mem[pos]));
		return (intptr_t)& mp.mem[pos];
	}
	else {
		pos = 0;
		printf("Out of memory.\n");
		return -1;
	}
}
void VM::addMacro(const std::string& var, const std::string& val) {
	macros[var] = val;
}
std::string VM::getMacros() {
	std::string out = "";
	for (auto m : macros)
		out += "#define " + m.first + " " + m.second + "\n";
	return out;
}
void VM::free(uint64_t start, uint64_t size) {
	auto end = start + size;
	for (auto& p : free_memory_positions) {
		if (p.end == start) {
			p.end += size;
			return;
		}
		else if (p.start == end) {
			p.start -= size;
			return;
		}
	}
	free_memory_positions.push_back({ start, end });
}

void VM::debug_push(const std::string& l) {
	debug_string_temp += l;
}
void VM::debug_declare_label() {
	debug_string += debug_string_temp + '\n';
	debug_push_start = mp.position;
	debug_string_temp = "";
}
void VM::debug_endline() {
	//show hex as first row
	//show code as second row
	for (int i = debug_push_start; i < debug_push_start + 8; ++i)
		if (i < mp.position)
			debug_string += Helper::int_to_hex(mp.mem[i]) + " ";
		else
			debug_string += " ";
	//show asm as third row
	debug_string += debug_string_temp;
	debug_string += '\n';
	debug_push_start = mp.position;
	debug_string_temp = "";
}
int VM::get_free_register_32() {
	int reg = -1;
	for (int i = 0; i < registers32.size(); ++i)
		if (registers32[i] == 0) {
			reg = i;
			break;
		}
	return reg;
};
int VM::get_free_register_64() {
	int reg = -1;
	for (int i = 0; i < registers64.size(); ++i)
		if (registers64[i] == 0) {
			reg = i;
			break;
		}
	return reg;
}
VMObject& VM::object(const VMClass & _class, const std::string & l) {
	objects[l] = VMObject();
	auto& o = objects[l];
	for (auto v : _class.variables) {
		Label label;
		if (std::holds_alternative<uint32_t>(v.second)) {
			auto str = std::get<uint32_t>(v.second);
			mp.push({ 0xB8 }); debug_push("mov eax, "); debug_endline();
			mp.push_fn_h(str); debug_push(std::to_string(str)); debug_endline();
		}
		else if (std::holds_alternative<uint64_t>(v.second)) {
			auto str = std::get<uint64_t>(v.second);
			mp.push({ 0x48, 0xB8 }); debug_push("movabs rax, "); debug_endline();
			mp.push_fn(str); debug_push(std::to_string(str)); debug_endline();
		}
		else if (std::holds_alternative<std::string>(v.second)) {
			auto str = std::get<std::string>(v.second);
			label = create_string(l + ':' + v.first, str);
		}
		else if (std::holds_alternative<Label>(v.second)) {
			auto label = std::get<Label>(v.second);
			//movabs rax, offset label
			//move a double into rax
			//mov rax, qword ptr [rax]
			//move a single into eax
			//mov eax, dword ptr [rax]
			if (label.basic == BasicType::i32) {
				size_t ptr = Helper::calculateCallOperand(&mp.mem[label.ptr], &mp.mem[mp.position + 1]);
				mp.push({ 0x8B, 0x05 }); debug_push("mov eax, dword ptr [rip + "); debug_endline();
				mp.push_fn_h(ptr); debug_push(Helper::int_to_hex(label.ptr)); debug_endline();
			}
			else if (label.basic == BasicType::i64) {
				mp.push({ 0x48, 0x8B, 0x05 }); debug_push("mov rax, qword ptr [rip + "); debug_endline();

			}
		}
		//mp.push({ 0x48, 0x83, 0xC4, 0x28 }); debug_push("add rsp, 40"); debug_endline();
		o.variables[v.first] = label;
	}
	return o;
}
VMClass VM::_class(const std::string & l) {
	classes[l] = VMClass();
	return classes[l];
}
uint32_t VM::getBytesUsed() {
	std::string out = "";
	for (auto l : labels)
		l.second.getMachineCode(*this, out);
	return out.size();
}
void VM::printAllLabels() {
	for (auto l : labels) {
		if (l.second.position == 0)
			continue;
		printf("Label:%s\n", l.first.data());
		if (l.second.debug.size() > 0)
			l.second.print(*this);
		else {
			std::string out = "";
			std::string out2 = "";
			l.second.getMachineCode(*this, out);
			for (auto val : out)
				out2 += Helper::int_to_hex(UCHAR(val)) + " ";
			printf("%s", out2.data());
		}
		printf("\n");
	}
}
std::string VM::getAllLabelsMachineCode() {
	std::string out = "";
	for (auto l : labels)
		if (l.second.position == 0)
			continue;
		else
			l.second.getMachineCode(*this, out);
	return out;
}
void VM::preserve(x64 reg) {
	if (reg == x64::a) {
		mp.push({ 0x48, 0x89, 0xC6 }); debug_push("mov rsi, rax");
	}
	else if (reg == x64::b) {
		mp.push({ 0x48, 0x89, 0xCE }); debug_push("mov rsi, rcx");
	}
	debug_endline();
}
void VM::recover(x64 reg) {
	if (reg == x64::a) {
		mp.push({ 0x48, 0x89, 0xF0 }); debug_push("mov rax, rsi");
	}
	else if (reg == x64::b) {
		mp.push({ 0x48, 0x89, 0xF1 }); debug_push("mov rcx, rsi");
	}
	debug_endline();
}
void VM::addTemplateFunction(const std::string & name, const std::string & mangled, const std::string & body, const std::vector<std::string>& args) {
	template_functions[name][mangled] = { body, args };
}
void VM::move(x64 a, x64 b) {
	if (a == x64::a) {
		if (b == x64::b) {
			mp.push({ 0x48, 0x89, 0xC1 }); debug_push("mov rcx, rax");
		}
		else if (b == x64::c) {
			mp.push({ 0x48, 0x89, 0xC2 }); debug_push("mov rdx, rax");
		}
		else if (b == x64::d) {
			mp.push({ 0x49, 0x89, 0xC0 }); debug_push("mov r8, rax");
		}
		else if (b == x64::e) {
			mp.push({ 0x49, 0x89, 0xC1 }); debug_push("mov r9, rax");
		}
		else if (b == x64::s) {
			mp.push({ 0x48, 0x89, 0xC6 }); debug_push("mov rsi, rax");
		}
	}
	else if (a == x64::b) {
		if (b == x64::a) {
			mp.push({ 0x48, 0x89, 0xC8 }); debug_push("mov rax, rcx");
		}
		else if (b == x64::c) {
			mp.push({ 0x48, 0x89, 0xCA }); debug_push("mov rdx, rcx");
		}
		else if (b == x64::d) {
			mp.push({ 0x49, 0x89, 0xC8 }); debug_push("mov r8, rcx");
		}
		else if (b == x64::e) {
			mp.push({ 0x49, 0x89, 0xC9 }); debug_push("mov r9, rcx");
		}
		else if (b == x64::s) {
			mp.push({ 0x48, 0x89, 0xCE }); debug_push("mov rsi, rcx");
		}
	}
	else if (a == x64::c) {
		if (b == x64::a) {
			mp.push({ 0x48, 0x89, 0xD0 }); debug_push("mov rax, rdx");
		}
		else if (b == x64::b) {
			mp.push({ 0x48, 0x89, 0xD1 }); debug_push("mov rcx, rdx");
		}
		else if (b == x64::d) {
			mp.push({ 0x49, 0x89, 0xD0 }); debug_push("mov r8, rdx");
		}
		else if (b == x64::e) {
			mp.push({ 0x49, 0x89, 0xD1 }); debug_push("mov r9, rdx");
		}
		else if (b == x64::s) {
			mp.push({ 0x48, 0x89, 0xD6 }); debug_push("mov rsi, rdx");
		}
	}
	else if (a == x64::d) {
		if (b == x64::a) {
			mp.push({ 0x4C, 0x89, 0xC0 }); debug_push("mov rax, r8");
		}
		else if (b == x64::b) {
			mp.push({ 0x4C, 0x89, 0xC1 }); debug_push("mov rcx, r8");
		}
		else if (b == x64::c) {
			mp.push({ 0x4C, 0x89, 0xC2 }); debug_push("mov rdx, r8");
		}
		else if (b == x64::e) {
			mp.push({ 0x4D, 0x89, 0xC1 }); debug_push("mov r9, r8");
		}
		else if (b == x64::s) {
			mp.push({ 0x4C, 0x89, 0xC6 }); debug_push("mov rsi, r8");
		}
	}
	else if (a == x64::e) {
		if (b == x64::a) {
			mp.push({ 0x4C, 0x89, 0xC8 }); debug_push("mov rax, r9");
		}
		else if (b == x64::b) {
			mp.push({ 0x4C, 0x89, 0xC9 }); debug_push("mov rcx, r9");
		}
		else if (b == x64::c) {
			mp.push({ 0x4C, 0x89, 0xCA }); debug_push("mov rdx, r9");
		}
		else if (b == x64::d) {
			mp.push({ 0x4D, 0x89, 0xC8 }); debug_push("mov r8, r9");
		}
		else if (b == x64::s) {
			mp.push({ 0x4C, 0x89, 0xCE }); debug_push("mov rsi, r9");
		}
	}
	else if (a == x64::s) {
		if (b == x64::a) {
			mp.push({ 0x48, 0x89, 0xF0 }); debug_push("mov rax, rsi");
		}
		else if (b == x64::b) {
			mp.push({ 0x48, 0x89, 0xF1 }); debug_push("mov rcx, rsi");
		}
		else if (b == x64::c) {
			mp.push({ 0x48, 0x89, 0xF2 }); debug_push("mov rdx, rsi");
		}
		else if (b == x64::d) {
			mp.push({ 0x49, 0x89, 0xF0 }); debug_push("mov r8, rsi");
		}
		else if (b == x64::e) {
			mp.push({ 0x49, 0x89, 0xF1 }); debug_push("mov r9, rsi");
		}
	}
	debug_endline();
}
void VM::move(Label a, int reg) {
	std::string a_reg = "eax";
	std::string ptr_string = "";
	std::string type = "dword";
	std::string out_string = "mov {}, {} PTR [rip + {}]";

	if (a.basic == BasicType::i32) {
		if (reg == -1) {
			reg = get_free_register_32();
			reg++; //skip rax for now
		}
		std::array<uint8_t, 2> code;
		//mov eax, DWORD PTR ii[rip]
		if (reg == 0) { //eax
			code = { 0x8B, 0x05 };
		}
		else if (reg == 1) { //ecx
			code = { 0x8B, 0x0D };
			a_reg = "ecx";
		}
		unsigned long ptr = Helper::calculateCallOperand(&mp.mem[a.ptr], &mp.mem[mp.position + 1]);
		mp.push({ code[0], code[1] }); mp.push_fn_h(ptr);
		ptr_string = Helper::int_to_hex(ptr);
	}
	else {
		if (reg == -1) {
			reg = get_free_register_64();
			reg++; //skip rax for now
		}
		type = "qword";
		std::array<uint8_t, 3> code;
		//mov rax, QWORD PTR ii[rip]
		if (reg == 0) { //rax
			code = { 0x48, 0x8B, 0x05 };
			a_reg = "rax";
		}
		else if (reg == 1) { //rcx
			code = { 0x48, 0x8D, 0x0D };
			//mp.push({ 0x48, 0x8D, 0x0D }); debug_push("lea rcx, [rip + ");
			a_reg = "rcx";
		}
		else if (reg == 2) { //rdx
			code = { 0x48, 0x8D, 0x15 };
			//mp.push({ 0x48, 0x8D, 0x0D }); debug_push("lea rcx, [rip + ");
			a_reg = "rdx";
		}
		unsigned long ptr = Helper::calculateCallOperand(&mp.mem[a.ptr], &mp.mem[mp.position + 2]);
		mp.push({ code[0], code[1], code[2] }); mp.push_fn_h(ptr);
		ptr_string = Helper::int_to_hex(ptr);
	}
	debug_push(fmt::format(out_string, a_reg, type, ptr_string));

}
void VM::move_offset(Label a, int reg, int offset) {
	//mov qword ptr [rax+0], offset animal_name //from icc, appears to not be working
																//lea rcx, [rip + cc]    from clang but probably not needed   
																//mov qword ptr[rax + 8], offset                   combined with the above line
																//also from clang
																//mov rcx, qword ptr [rip + cc]
																//mov qword ptr [rax], rcx
	std::array<uint8_t, 3> code = { 0x48, 0xC7, 0x00 };
	std::string text = "";
	std::string type = "qword";
	std::string out_string = "mov {} ptr [{}{}], offset {}";
	std::string b_reg = "rax";
	std::string offset2 = "";
	if (reg == 0) {
		//code[2] += 1;
	}
	else if (reg == 1) {
		code[2] += 1;
		b_reg = "rcx";
	}
	else if (reg == 2) {
		code[2] += 2;
		b_reg = "rdx";
	}
	else if (reg == 3) {
		code[0] += 1;
		b_reg = "r8";
	}
	else if (reg == 4) {
		code[0] += 1;
		code[2] += 1;
		b_reg = "r9";
	}
	if (offset > 128) {
		code[2] += 80;
		mp.push({ code[0], code[1], code[2] }); mp.push_fn_h(offset);
		offset2 = " + " + Helper::int_to_hex(offset);
	}
	else if (offset > 0) {
		code[2] += 40;
		mp.push({ code[0], code[1], code[2] }); mp.push(offset);
		offset2 = " + " + Helper::int_to_hex(offset);
	}
	else {
		mp.push({ code[0], code[1], code[2] });
	}
	unsigned long ptr = Helper::calculateCallOperand(&mp.mem[a.ptr], &mp.mem[mp.position - 1]);
	mp.push_fn_h(4294967263);
	debug_push(fmt::format(out_string, type, b_reg, offset2, Helper::int_to_hex(ptr)));
	debug_endline();
}
void VM::move(x64 a, std::pair<x64, uint32_t> b) {
	std::array<uint8_t, 3> code;
	std::string text = "";
	std::string out_string = "mov qword ptr [{}{}],{}";
	std::string a_reg = "rax";
	std::string b_reg = "rax";
	std::string offset = "";
	if (a == x64::a) { //rax
		code = { 0x48, 0x89, 0x00 };
	}
	else if (a == x64::b) {
		code = { 0x48, 0x89, 0x08 };
		a_reg = "rcx";
	}
	else if (a == x64::c) {
		code = { 0x48, 0x89, 0x10 };
		a_reg = "rdx";
	}
	else if (a == x64::d) {
		code = { 0x4C, 0x89, 0x00 };
		a_reg = "r8";
	}
	else if (a == x64::e) {
		code = { 0x4C, 0x89, 0x08 };
		a_reg = "r9";
	}
	if (b.first == x64::a) {
		//code[2] += 1;
	}
	else if (b.first == x64::b) {
		code[2] += 1;
		b_reg = "rcx";
	}
	else if (b.first == x64::c) {
		code[2] += 2;
		b_reg = "rdx";
	}
	else if (b.first == x64::d) {
		code[0] += 1;
		b_reg = "r8";
	}
	else if (b.first == x64::e) {
		code[0] += 1;
		code[2] += 1;
		b_reg = "r9";
	}
	else if (b.first == x64::s) {
		code[2] += 6;
		b_reg = "rsi";
	}
	if (b.second > 128) {
		code[2] += 80;
		mp.push({ code[0], code[1], code[2] }); mp.push_fn_h(b.second);
		offset = " + " + Helper::int_to_hex(b.second);
	}
	else if (b.second > 0) {
		code[2] += 40;
		mp.push({ code[0], code[1], code[2] }); mp.push(b.second);
		offset = " + " + Helper::int_to_hex(b.second);
	}
	else {
		mp.push({ code[0], code[1], code[2] });
	}
	debug_push(fmt::format(out_string, b_reg, offset, a_reg));
	debug_endline();
}
void VM::move(std::pair<x64, uint32_t> a, x64 b) {
	std::array<uint8_t, 3> code;
	std::string text = "";
	std::string out_string = "mov {}, qword ptr [{}{}]";
	std::string a_reg = "rax";
	std::string b_reg = "rax";
	std::string offset = "";
	if (b == x64::a) { //rax
		code = { 0x48, 0x8B, 0x00 };
	}
	else if (b == x64::b) {
		code = { 0x48, 0x8B, 0x08 };
		b_reg = "rcx";
	}
	else if (b == x64::c) {
		code = { 0x48, 0x8B, 0x10 };
		b_reg = "rdx";
	}
	else if (b == x64::d) {
		code = { 0x4C, 0x8B, 0x00 };
		b_reg = "r8";
	}
	else if (b == x64::e) {
		code = { 0x4C, 0x8B, 0x08 };
		b_reg = "r9";
	}
	if (a.first == x64::a) {
		//code[2] += 1;
	}
	else if (a.first == x64::b) {
		code[2] += 1;
		a_reg = "rcx";
	}
	else if (a.first == x64::c) {
		code[2] += 2;
		a_reg = "rdx";
	}
	else if (a.first == x64::d) {
		code[0] += 1;
		a_reg = "r8";
	}
	else if (a.first == x64::e) {
		code[0] += 1;
		code[2] += 1;
		a_reg = "r9";
	}
	if (a.second > 128) {
		code[2] += 80;
		mp.push({ code[0], code[1], code[2] }); mp.push_fn_h(a.second);
		offset = " + " + Helper::int_to_hex(a.second);
	}
	else if (a.second > 0) {
		code[2] += 40;
		mp.push({ code[0], code[1], code[2] }); mp.push(a.second);
		offset = " + " + Helper::int_to_hex(a.second);
	}
	else {
		mp.push({ code[0], code[1], code[2] });
	}
	debug_push(fmt::format(out_string, b_reg, a_reg, offset));
	debug_endline();
}
//create overloads
//move value into a register, can create ints, everything else needs to already be on stack and passed with a Label
//void VM::move(std::pair<_reg, uint8_t> a, Label b);
//void VM::move(std::pair<_reg, uint8_t> a, int32_t b);
//when creating new variable on stack larger than int32 use movabs first
//strings get created on the heap
//move a value into a free register
void VM::move(size_t value) {
	int reg = -1;
	if (value < INT32_MAX) {
		reg = get_free_register_32();
	}
	else {
		reg = get_free_register_64();
		switch (reg) {
		case 0:
#ifdef WIN32 //todo: change to x64, calls should be the same on x86_64 regardless of OS
			mp.push({ 0x48, 0xB8 }); debug_push("lea rax,");
#endif
			break;
		case 1:
#ifdef WIN32
			mp.push({ 0x48, 0xB9 }); debug_push("movabs rcx,");
#endif
			break;
		}
		mp.push_fn(value); debug_push(func);
		debug_endline();
	}
}
void VM::move_i32(size_t value) {
	int reg = get_free_register_64();
	size_t ptr = (size_t)&mp.mem[value];
	switch (reg) {
#ifdef WIN32 //todo: change to x64, calls should be the same on x86_64 regardless of OS
	case 0:
		ptr = Helper::calculateCallOperand(&mp.mem[value], &mp.mem[mp.position]);
		mp.push({ 0x8B, 0x0D }); debug_push("mov ecx, dword ptr[rip + i]"); debug_endline();
		break;
	case 1:
		mp.push({ 0x48, 0x8D, 0x15 }); debug_push("lea rdx, [rip + ");
		break;
#endif
	}
	mp.push_fn_h(ptr); debug_push(Helper::int_to_hex(ptr) + "]"); debug_endline();
}
void VM::move_string(size_t value, int r) {
	int reg = r;
	if (reg == -1)
		reg = get_free_register_64();

	size_t ptr = (size_t)&mp.mem[value];
	switch (reg) {
#ifdef WIN32 //todo: change to x64, calls should be the same on x86_64 regardless of OS
	case 0:
		//mp.push({ 0x48, 0xB9 }); debug_push("movabs rcx, offset "); //works, not relative
		ptr = Helper::calculateCallOperand(&mp.mem[value], &mp.mem[mp.position + 2]);
		mp.push({ 0x48, 0x8D, 0x0D }); debug_push("lea rcx, [rip + ");
		//mp.push({ }); debug_push("mov rax, qword ptr [rip + "); //find difference for mov and lea
		break;
	case 1:
		mp.push({ 0x48, 0x8D, 0x15 }); debug_push("lea rdx, [rip + ");
		//mp.push({ 0x48, 0xC7 }); debug_push("mov rcx, qword ptr [rip + "); //find difference for mov and lea
		break;
#endif
	}
	mp.push_fn_h(ptr); debug_push(Helper::int_to_hex(ptr) + "]"); debug_endline();
	//mp.push({ 0x48, 0x8B, 0x08 }); debug_push("mov rcx, qword ptr [rax]"); debug_endline();
}
/*void VM::move(Label & label) {
	if (label.basic == BasicType::str)
		move_string(label.ptr);
	else if (label.basic == BasicType::i32)
		move_i32(label.ptr);
}*/
//move a value into a free register
void VM::move_abs(size_t value) {
	int reg = get_free_register_64();
	switch (reg) {
	case 0:
#ifdef WIN32 //todo: change to x64, calls should be the same on x86_64 regardless of OS
		mp.push({ 0x48, 0xB8 }); debug_push("movabs rax,");
#endif
		break;
	case 1:
#ifdef WIN32
		mp.push({ 0x48, 0xB9 }); debug_push("movabs rcx,");
#endif
		break;
	}
	mp.push_fn(value); debug_push(func);
	debug_endline();
}
//moves a value into a ptr
void VM::move(uint8_t* value, size_t ptr) {

};
//moves an i32 into a label
void VM::move(int32_t value, Label label) { //mov qword ptr [rip + i2], rax
};
//moves an i64 into a label
void VM::move(int64_t value, Label label) { //movabs rax, value | mov qword ptr [rip + i2], rax
}
//moves an i32 into a register
void VM::move(int32_t value, x64 reg) { //movabs reg, value
	std::array<uint8_t, 3> code;
	std::string text = "";
	std::string out_string = "movabs {}, {}";
	std::string a_reg = "rax";
	std::string offset = "";
	if (reg == x64::a) { //rax
		code = { 0x48, 0xC7, 0xC0 };
	}
	else if (reg == x64::b) {
		code = { 0x48, 0xC7, 0xC1 };
		a_reg = "rcx";
	}
	else if (reg == x64::c) {
		code = { 0x48, 0xC7, 0xC2 };
		a_reg = "rdx";
	}
	else if (reg == x64::d) {
		code = { 0x49, 0xC7, 0xC0 };
		a_reg = "r8";
	}
	else if (reg == x64::e) {
		code = { 0x49, 0xC7, 0xC1 };
		a_reg = "r9";
	}
	mp.push({ code[0], code[1], code[2] });
	mp.push_fn_h(value);
	debug_push(fmt::format(out_string, a_reg, std::to_string(value)));
	debug_endline();
}
//moves an i64 into a register
void VM::move(int64_t value, x64 reg) { //movabs reg, value
	std::array<uint8_t, 2> code;
	std::string text = "";
	std::string out_string = "movabs {}, {}";
	std::string a_reg = "rax";
	std::string offset = "";
	if (reg == x64::a) { //rax
		code = { 0x48, 0xB8 };
	}
	else if (reg == x64::b) {
		code = { 0x48, 0xB9 };
		a_reg = "rcx";
	}
	else if (reg == x64::c) {
		code = { 0x48, 0xBA };
		a_reg = "rdx";
	}
	else if (reg == x64::d) {
		code = { 0x49, 0xB8 };
		a_reg = "r8";
	}
	else if (reg == x64::e) {
		code = { 0x49, 0x89 };
		a_reg = "r9";
	}
	mp.push({ code[0], code[1] });
	mp.push_fn(value);
	debug_push(fmt::format(out_string, a_reg, std::to_string(value)));
	debug_endline();
}
void VM::move(uint64_t value, x64 reg) {
	move((int64_t)value, reg);
}
//creates a ptr and assigns an i32 to it
Label& VM::create_i32(const std::string & name, int32_t v) {
	return label(name, (uint32_t)v);
}
Label& VM::create_ui64(const std::string & name, uint64_t v) {
	return label(name, v);
}
Label& VM::create_string(const std::string & name, const std::string & v) {
	return label(name, v);
}
//adds two values at pointers
void VM::add_i32(uint8_t* a, uint8_t* b) {
};
//creates a blank pointer
size_t VM::create_ptr() {
	return 1;
};
//call a function
void VM::call_near(uint8_t* fn) {
	auto v = Helper::calculateCallOperand(fn, &mp.mem[mp.position]); mp.push({ 0xE8 }); debug_push("call,"); mp.push_fn_h(v); debug_push(func); debug_endline();
}
void VM::call_near_from(uint8_t* fn, int from) {
	auto v = Helper::calculateCallOperand(fn, &mp.mem[from]); mp.push({ 0xE8 }); debug_push("call,"); mp.push_fn_h(v); debug_push(func); debug_endline();
}
void VM::call_far(size_t fn) {
	move_abs(fn);
	mp.push({ 0xFF, 0xD0 }); debug_push("call, rax"); debug_endline();
}
void VM::move(_reg r) {
	//swap registers if needed, or do nothing
	//check what register we are on
	//check if this passed reg is same position or needs to be swapped
	if (r.val > 16) {
		//no more registers left, move onto stack
		//mov qword ptr [rsp + offset], r

	}
	else if (r.type == RegType::x32) {
		int free_reg = get_free_register_32();
		if (r.val == free_reg) {
			//do nothing, arg will remain in same register for next function
		}
		else {
			swap_reg32(free_reg, r.val);
		}
	}
	else {

	}
}
//can use static assert to make sure specific types get passed
//static_assert(std::is_same<decltype(retval), bool>::value, "retval must be bool");
void VM::get_value(std::variant<size_t, std::string, Label, nullobj, _reg> v) {
	if (std::holds_alternative<Label>(v)) {
		//this is a label
		auto label = std::get<Label>(v);
		move(label);
	}
	else if (std::holds_alternative<size_t>(v)) {
		//puts v onto the next available register
		//example first run
		//mov ecx, 1
		//mov edx, 1
		move(std::get<size_t>(v));
	}
	else if (std::holds_alternative<std::string>(v)) {

	}
	else if (std::holds_alternative<nullobj>(v)) {
		//do nothing
	}
	else if (std::holds_alternative<_reg>(v)) {
		move(std::get<_reg>(v));
	}
}
void VM::swap_reg32(int a, int b) {
	if (a == 0) {
		if (b == 1) {//mov edx, ecx or move reg 0 into reg 1
			mp.push({ 0x89, 0xCA }); debug_push("mov edx, ecx"); debug_endline();
		}

	}
	else if (a == 1) {
		if (b == 0) {//mov ecx, edx or move reg 1 into reg 0
			mp.push({ 0x89, 0xD1 }); debug_push("mov edx, ecx"); debug_endline();
		}

	}
}
void VM::ret() {
	mp.push({ 0xC3 }); debug_push("ret"); debug_endline();
}
void VM::ret(std::variant<Label, uint8_t, uint16_t, uint32_t, uint64_t, std::string> v) {
	if (std::holds_alternative<uint32_t>(v)) {
		auto str = std::get<uint32_t>(v);
		mp.push({ 0xB8 }); debug_push("mov eax, "); debug_endline();
		mp.push_fn_h(str); debug_push(std::to_string(str)); debug_endline();
	}
	else if (std::holds_alternative<uint64_t>(v)) {
		auto str = std::get<uint64_t>(v);
		mp.push({ 0x48, 0xB8 }); debug_push("movabs rax, "); debug_endline();
		mp.push_fn(str); debug_push(std::to_string(str)); debug_endline();
	}
	else if (std::holds_alternative<std::string>(v)) {
		auto str = std::get<std::string>(v);
		size_t ptr = create_string(str, str).ptr;
		ptr = Helper::calculateCallOperand(&mp.mem[ptr], &mp.mem[mp.position + 2]);
		mp.push({ 0x48, 0x8D, 0x05 }); debug_push("lea rax, [rip + "); debug_endline();
		mp.push_fn_h(ptr); debug_push(str); debug_endline();
	}
	else if (std::holds_alternative<Label>(v)) {
		auto label = std::get<Label>(v);
		//movabs rax, offset label
		//move a double into rax
		//mov rax, qword ptr [rax]
		//move a single into eax
		//mov eax, dword ptr [rax]
		if (label.basic == BasicType::i32) {
			size_t ptr = Helper::calculateCallOperand(&mp.mem[label.ptr], &mp.mem[mp.position + 1]);
			mp.push({ 0x8B, 0x05 }); debug_push("mov eax, dword ptr [rip + "); debug_endline();
			mp.push_fn_h(ptr); debug_push(Helper::int_to_hex(label.ptr)); debug_endline();
		}
		else if (label.basic == BasicType::i64) {
			mp.push({ 0x48, 0x8B, 0x05 }); debug_push("mov rax, qword ptr [rip + "); debug_endline();

		}
	}
	//mp.push({ 0x48, 0x83, 0xC4, 0x28 }); debug_push("add rsp, 40"); debug_endline();
	mp.push({ 0xC3 }); debug_push("ret"); debug_endline();
}
void VM::call(x64 reg) {
	std::array<uint8_t, 2> code;
	std::string out_string = "call {}";
	if (reg == x64::a) { //rax
		mp.push({ 0xFF, 0xD0 }); debug_push("call rax");
	}
	else if (reg == x64::b) {
		mp.push({ 0xFF, 0xD1 }); debug_push("call rcx");
	}
	else if (reg == x64::c) {
		mp.push({ 0xFF, 0xD2 }); debug_push("call rdx");
	}
	else if (reg == x64::d) {
		mp.push({ 0x41, 0xFF, 0xD0 }); debug_push("call r8");
	}
	else if (reg == x64::e) {
		mp.push({ 0x41, 0xFF, 0xD1 }); debug_push("call r9");
	}
	debug_endline();
}
void VM::call(const std::string& fn) {
	//const int size = sizeof...(args) + 2;
	//int res[size] = { 1,args...,2 };
	//static_assert(std::conjunction<std::is_integral<Ints>...>::value);
	//this mgiht be a hack, not sure
	auto label = labels[fn].ptr;
	func = fn;
	if (label > UINT32_MAX)
		call_far(label);
	else
		call_near(&mp.mem[label]);
	func = "";
	clear_registers();
}
Label& VM::label(const std::string& l, BasicType basic) {
	//if (l == "_i32_add2_i32")
		//printf("test");
	auto got = labels.find(l);
	if (got != labels.end())
		return got->second;
	debug_push(l + ":");
	labels[l].name = l;
	uint64_t val = 0;
	auto ptr = alloc(250, val);
	//auto test = (uint64_t)&mp.mem[val];
	//std::memcpy((mp.mem + val), &ptr, sizeof(ptr));
	labels[l].ptr = val;
	labels[l].basic = basic;
	debug_declare_label();
	return labels[l];
}
Label& VM::label(const std::string& l, std::vector<uint8_t> code) {
	auto& _label = label(l, BasicType::i32);
	_label.position = 0; //reset position
	mp.fill(_label, code);
	return _label;
}
Label& VM::label(const std::string & l, std::variant<uint8_t, uint16_t, uint32_t, uint64_t, int8_t, int16_t, int32_t, int64_t, double, float, std::string> v) {
	if (std::holds_alternative<uint32_t>(v)) {
		auto& _label = label(l, BasicType::u32);
		_label.position = 0; //reset position
		auto str = std::get<uint32_t>(v);
		mp.fill(_label, str); debug_push(":" + l); debug_endline();
		return _label;
	}
	else if (std::holds_alternative<uint64_t>(v)) {
		auto& _label = label(l, BasicType::u64);
		_label.SetType({ BasicType::u64, "u64" });
		_label.by = By::val;
		_label.position = 0; //reset position
		auto str = std::get<uint64_t>(v);
		mp.fill(_label, str); debug_push(":" + l); debug_endline();
		return _label;
	}
	else if (std::holds_alternative<float>(v)) {
		auto& _label = label(l, BasicType::f32);
		_label.SetType({ BasicType::f32, "f32" });
		_label.by = By::val;
		_label.position = 0; //reset position
		auto str = std::get<float>(v);
		mp.fill(_label, str); debug_push(":" + l); debug_endline();
		return _label;
	}
	else if (std::holds_alternative<double>(v)) {
		auto& _label = label(l, BasicType::f64);
		_label.SetType({ BasicType::f64, "f64" });
		_label.by = By::val;
		_label.position = 0; //reset position
		auto str = std::get<double>(v);
		mp.fill(_label, str); debug_push(":" + l); debug_endline();
		return _label;
	}
	else if (std::holds_alternative<int64_t>(v)) {
		auto& _label = label(l, BasicType::i64);
		_label.SetType({ BasicType::i64, "i64" });
		_label.by = By::val;
		_label.position = 0; //reset position
		auto str = std::get<int64_t>(v);
		//*(intptr_t*)(vm.mp.mem + vm.mp.position) = (intptr_t)do_nothing;
		//vm.mp.position += sizeof(intptr_t);
		mp.fill(_label, str); debug_push(":" + l); debug_endline();
		return _label;
	}
	else if (std::holds_alternative<uint16_t>(v)) {
		auto& _label = label(l, BasicType::u16);
		_label.position = 0; //reset position
		auto str = std::get<uint16_t>(v);
		mp.fill(_label, str); debug_push(":" + l); debug_endline();
		return _label;
	}
	else if (std::holds_alternative<uint8_t>(v)) {
		auto& _label = label(l, BasicType::u8);
		_label.position = 0; //reset position
		auto str = std::get<uint8_t>(v);
		mp.fill(_label, str); debug_push(":" + l); debug_endline();
		return _label;
	}
	else if (std::holds_alternative<std::string>(v)) {
		auto& _label = label(l, BasicType::str);
		_label.SetType({ BasicType::str, "string" });
		_label.by = By::val;
		_label.position = 0; //reset position
		auto str = std::get<std::string>(v);
		uint64_t val = 0;
		auto ptr = alloc(24, val);
		mp.fill(_label, ptr); debug_push(":" + l); debug_endline();
		string* s = (string*)(*(intptr_t*)&mp.mem[_label.ptr]);
		str += '\0';
		s->size = str.size();
		s->capacity = str.size();
		s->data = (char*)alloc(str.size(), val);
		std::memcpy(s->data, str.data(), str.size());
		return _label;
	}
	return null_label;
}
void DoNothing() {
}
VM::variant_func VM::Run(const std::string & l) {
	auto got = labels.find(l);
	if (got != labels.end()) {
		auto _print = reinterpret_cast<variant_func>(&mp.mem[got->second.ptr]);
		return _print;
	}
	else {
		printf("Function(%s) does not exist.\n", l.data());
		return reinterpret_cast<variant_func>(DoNothing);
	}
}
void VM::print() {
	printf("%s", debug_string.data());
}
//returns ptr at label
Label& VM::get_label(const std::string& l) {
	auto got = labels.find(l);
	if (got != labels.end())
		return got->second;
	else
		return null_label;
}
//returns ptr at register
size_t VM::get_register(const std::string& l) {
	return 1;
}
//todo: implment const
//run through the code, replace const funcs and loops with a const value
void VM::optimize() {

}

void Label::ret(VM& vm) {
	vm.mp.fill(*this, (uint8_t)0xC3); vm.debug_push("ret"); vm.debug_endline();
}

void Label::call_near(VM & vm, const std::string & func) {
	auto label = vm.labels[func].ptr;
	auto v = Helper::calculateCallOperand(&vm.mp.mem[label], &vm.mp.mem[ptr + position]); vm.mp.fill(*this, (uint8_t)0xE8); vm.debug_push("call,");
	std::memcpy((vm.mp.mem + ptr + position), &v, sizeof v);
	position += sizeof v;
	vm.debug_push(func); vm.debug_endline();

}
		
void Label::call_far(VM & vm, const std::string & func) {
	auto label = vm.labels[func].ptr;
	vm.mp.fill(*this, { 0x48, 0xB8 }); vm.debug_push("movabs rax,");
	std::memcpy((vm.mp.mem + ptr + position), &label, sizeof label);
	position += sizeof label;
	vm.debug_push(func);
	vm.debug_endline();
	vm.mp.fill(*this, { 0xFF, 0xD0 }); vm.debug_push("call, rax"); vm.debug_endline();
}

void Label::move(VM & vm, int32_t value, x64 reg) {
	std::array<uint8_t, 3> code;
	std::string text = "";
	std::string out_string = "movabs {}, {}";
	std::string a_reg = "rax";
	std::string offset = "";
	if (reg == x64::a) { //rax
		code = { 0x48, 0xC7, 0xC0 };
	}
	else if (reg == x64::b) {
		code = { 0x48, 0xC7, 0xC1 };
		a_reg = "rcx";
	}
	else if (reg == x64::c) {
		code = { 0x48, 0xC7, 0xC2 };
		a_reg = "rdx";
	}
	else if (reg == x64::d) {
		code = { 0x49, 0xC7, 0xC0 };
		a_reg = "r8";
	}
	else if (reg == x64::e) {
		code = { 0x49, 0xC7, 0xC1 };
		a_reg = "r9";
	}
	vm.mp.fill(*this, { code[0], code[1], code[2] }); position += 3;
	std::memcpy((vm.mp.mem + ptr + position), &value, sizeof value);
	position += sizeof value;
	vm.debug_push(fmt::format(out_string, a_reg, std::to_string(value)));
	vm.debug_endline();
}

void Label::print(VM & vm) {
	printf("%s\n", debug.data());
}

void Label::getMachineCode(VM& vm, std::string& mc) {
	for (int i = ptr; i < ptr + position; ++i) {
		mc += int(vm.mp.mem[i]);
	}
}

std::string class_def::get_def(VM& vm, const std::vector<std::string>& template_vars) {
	if (is_template)
		return "";
	if (name.size() == 0)
		return "";
	if (name == "$")
		return "";
	std::string out = "";
	//out += add_types(vm);
	if ((int)basic < 50)
		return out;
	if (is_enum)
		out += "typedef enum _" + name + "{";
	else
		out += "typedef struct _" + name + "{";
	if (is_enum) {
		for (auto v : variables) {
			auto i = std::get<uint64_t>(v.second.value);
			out += name + "$" + v.first + " = " + std::to_string(i) + ',';
		}
		out += "}" + name + ";";
	}
	else {
		out += get_variables_def(vm, template_vars);
		out += "}" + name + ";";
	}
	return out;
}
std::string class_def::add_types(VM& vm) {
	std::string out = "";
	for (auto v : variables) {
		if (v.second.type == BasicType::var || v.second.type == BasicType::str) {
			auto c = vm.FindClass(v.second.GetType());
			out += c->get_def(vm);
		}
	}
	if (auto p = GetParentClass(vm))
		out += p->add_types(vm);
	return out;
}
tsl::ordered_map<std::string, Type> class_def::get_variables(VM& vm, const std::vector<std::string>& template_vars) {
	tsl::ordered_map<std::string, Type> vec;// = variables;
	if (auto p = GetParentClass(vm)) {
		auto v = p->get_variables(vm);
		vec.insert(v.begin(), v.end());
	}
	vec.insert(variables.begin(), variables.end());
	return vec;
}
std::string class_def::get_variables_def(VM& vm, const std::vector<std::string>& template_vars) {
	std::string out = "";
	tsl::ordered_map<std::string, Type> vars;
	if (auto p = GetParentClass(vm))
		vars = p->get_variables(vm);
	else
		vars = variables;

	for (auto v : vars) {
		if (v.second.type == BasicType::var) {
			/*if (template_vars.size() > 0) {
				out += template_vars.front() + "* ";
			}
			else {*/
				if (v.second.isNativeArray)
					out += v.second.GetComplexType() + "* ";
				else
					out += v.second.GetComplexType() + " ";
			//}
		}
		else {
			std::string temp = "";
			if (v.second.isNativeArray)
				temp = "*";
			out += v.second.GetComplexType() + temp + " ";
		}
		out += v.first + ";\n";
	}
	return out;
}

class_def* class_def::GetParentClass(VM& vm) {
	if (extends.size() > 0) {
		if (auto c = vm.FindClass(extends)) {
			return c;
		}
		printf("Unable to extend class %s. Not found.\n", extends.data());
	}
	return nullptr;
}

Type* class_def::FindVariable(VM& vm, const std::string& c, class_def* parent) {
	auto got = variables.find(c);
	if (got != variables.end()) {
		parent = this;
		return (Type*)&got->second;
	}
	if (auto p = GetParentClass(vm))
		return p->FindVariable(vm, c, parent);
	return nullptr;
}

Func* class_def::FindFunction(VM& vm, const std::string& c, class_def* parent) {
	auto got = functions.find(c);
	if (got != functions.end()) {
		parent = this;
		return &got->second;
	}
	if (auto p = GetParentClass(vm))
		return p->FindFunction(vm, c, parent);
	return nullptr;
}
struct TEMP_OUT {
	std::string args = "";
	std::string names = "";
	std::string types = "";
	std::string mangled = "";
};

TEMP_OUT Mangle2(DemangledStruct& demangled) {
	auto type_check = [](const std::string& str) {
		if (!Helper::BasicTypeCheck(str))
			return "*";
		else
			return "";
	};
	TEMP_OUT out;
	out.args = demangled.name + "$(";
	out.names = "(";
	out.types = "(";
	out.mangled = "_" + demangled.type.type_name + "_" + demangled.name + "_";
	for (int i = 0; i < demangled.args.size(); ++i) {
		out.args += "o" + std::to_string(i) + ":" + demangled.args[i]->name + ",";
		out.names += "o" + std::to_string(i) + ",";
		out.types += demangled.args[i]->name + type_check(demangled.args[i]->name) + ",";
		out.mangled += demangled.args[i]->name + "_";
	}
	if (demangled.args.size() > 0) {
		out.args.pop_back();
		out.names.pop_back();
		out.types.pop_back();
		out.mangled.pop_back();
	}
	//mangled += "):";
	out.args += "):" + demangled.type.type_name + ":";
	out.names += ")";
	out.types += ")";
	out.args += demangled.body;
	out.mangled += "$";
	return out;
}
void class_def::AddFunction(VM& vm, Func func) {
	functions[func.name] = func;
	auto& vptr = vm.label(func.func->name, std::vector<uint8_t>{ 0 });
	auto str = Helper::Demangle(func.func->name, {}, "");
	auto t = func.func->GetType();
	auto t2 = Helper::ModifyMangled(func.func->name, str);
	vptr.def = "extern " + t.GetComplexType() + " " + t2.first  + "(" + t2.second + ");" + t2.first + ";" + std::to_string(vptr.ptr) + "\n";
	//vptr.def = t.GetComplexType() + "(*" + t2.first + ")(" + t2.second + ") = (void*)0x0;" + t2.first + ";" + std::to_string(vptr.ptr) + "\n"; //possibly use libName.ptr
	if (extends.size() > 0 && name != func.name) {
		//Go all the way up and find what classes have this func
		std::vector<Func> classes;
		if (auto p = GetParentClass(vm)) {
			auto got_func = p->functions.find(func.name);
			if (got_func != p->functions.end()) {
				classes.push_back(got_func->second);
			}
		}
		//std::string code = "switch\n";
		/*
		switch(v)
>  case 0
>   return update()
>  case 1
>   return update2()
> return 1
*/
//func only needs to be created in most furthest parent
		if (classes.size() > 0) {
			auto back = classes.back();
			auto str = Helper::Demangle(back.func->name, {}, "");
			auto test = Mangle2(str);

			std::string code = test.args + "\n switch(o0.VTABLE)\n";
			code += "  case 0\n";
			code += "   return _u8_talk_Animal(o0)\n";
			code += "  case 1\n";
			code += "   return _u8_talk_Duck(o0)\n";
			code += "  default\n";
			code += "   return 31\n";
			code += "\n";
			auto& vptr = vm.label(test.mangled, std::vector<uint8_t>{ 0 });
			auto v = back.func->GetType();
			auto t = Helper::ModifyMangled(back.func->name, str);
			//vptr.def = v.GetComplexType() + "(*" + t.first + "$)(" + t.second + ") = (void*)0x0;" + t.first + "$;" + std::to_string(vptr.ptr) + "\n"; //possibly use libName.ptr
			vptr.def = "extern " + v.GetComplexType() + " " + test.mangled + test.types + ";" + test.mangled + ";" + std::to_string(vptr.ptr) + "\n";
			//vm.labels.emplace("_u8_talk_Animal_", vptr);
			//functions[func.name].func->ptr = vptr;
			Parser::queueMachineCode(vm, code);
		}
	}
		//SetVirtual(vm, func.name);
}
#include "parser.h"
void class_def::SetVirtual(VM& vm, const std::string& str) {
	//start with parent and start adding functions to switch, only if parent has same function declared
	/*if (auto p = GetParentClass(vm))
		p->SetVirtual(vm, str);
	auto got_func = functions.find(str);
	if (got_func != functions.end()) {
		got_func->second.is_virtual = true;
		std::string virt_func = "";
		void (*Base_Vtable[])() = { &Base_Update, &Base_Draw, &Base_ToString };
		//Create a virtual function for this function and set the funtion to call 
		FunctionToken func;
		//func.type = { BasicType::var, "void (*Base_Vtable[])()" };
		std::string code = "switch\n";
		//for every child class, add its function to the switch
		for (auto f : functions) {
			code += "case " + function_id + ":" + "return " + function_name;
		}
		func.type = got_func->second.func->GetType();
		func.symbols.push_back({ SymbolType::Function, "test_func" });
		func.symbols.push_back({SymbolType::Function, "test_func"});
		Parser::getMachineCode();
		auto body = vm.label("$" + got_func->second.func->name, std::vector<uint8_t>{ 0 });
		auto vptr = vm.label("$" + got_func->second.func->name, std::vector<uint8_t>{ 0 }).ptr;
		got_func->second.func->ptr = vptr;
	}*/
}

bool class_def::HasParent(VM& vm, const std::string& p) {
	if (auto p2 = GetParentClass(vm)) {
		if (p2->name == p)
			return true;
		return p2->HasParent(vm, p);
	}
	return false;
}

class_def* class_def::BuildClassFromTemplate(VM& vm, const std::string& str) {
	auto _class = vm.FindClass(str);
	auto _newclass = *this;
	_newclass.name = name + '$' + str;
	for (auto vv : _newclass.variables) {
		auto& v = _newclass.variables[vv.first];
		if (v.type == BasicType::template_type) {
			v.type = _class->basic;
			v.type_name = str;
		}

		
	}
	vm.class_defs[name + '$' + str] = std::move(_newclass);
	return &vm.class_defs[name + '$' + str];
}
