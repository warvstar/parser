#include "vm.h"
#include "helper.h"
#include "fmt/format.h"
using namespace fmt;

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
void VM::AddBlock(CodeBlock& block) {

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
size_t VM::create_i32(int32_t v) {
	return 1;
}
Label VM::create_string(const std::string & name, const std::string & v) {
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
Label VM::label(const std::string& l, BasicType basic) {
	debug_push(l + ":");
	labels[l].ptr = mp.position;
	labels[l].basic = basic;
	debug_declare_label();
	return labels[l];
}
Label VM::label(const std::string & l, std::variant<uint8_t, uint16_t, uint32_t, uint64_t, std::string> v) {
	if (std::holds_alternative<uint32_t>(v)) {
		auto _label = label(l, BasicType::i32);
		auto str = std::get<uint32_t>(v);
		mp.push_fn_h(str); debug_push(":" + l); debug_endline();
		return _label;
	}
	else if (std::holds_alternative<uint64_t>(v)) {
		auto _label = label(l, BasicType::i64);
		auto str = std::get<uint64_t>(v);
		mp.push_fn((size_t)str); debug_push(":" + l); debug_endline();
		return _label;
	}
	else if (std::holds_alternative<std::string>(v)) {
		auto _label = label(l, BasicType::str);
		auto str = std::get<std::string>(v);
		mp.push_string(str); debug_push(":" + l); debug_endline();
		return _label;
	}
	return Label();
	//auto t = ForceCast<size_t>(str);
	//auto t1 = ForceCast<size_t>(v);
	//auto t2 = std::get<size_t>(v);
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

void CodeBlock::Set(const std::string& n, std::variant<size_t, Label, _reg, nullobj> v) {
	registers.push_back(std::make_shared<VirtualReg>(order++, registers.size() + 1, n, v));
}
void CodeBlock::Add(const std::string& n, std::variant<size_t, Label, _reg, nullobj> v) {
	auto last = gets.back().get();
	last->type = SymbolType::Plus;
	Get(n, v);
	auto next = gets.back().get();
	last->depends_on.push_back(next);
	next->dependant_on.push_back(last);
}
void CodeBlock::Set(const std::string & n) {
	registers.push_back(std::make_shared<VirtualReg>(order++, registers.size() + 1, n, nullobj()));
}
VirtualHandle CodeBlock::Set(VirtualReg reg) {
	printf("Creating virtual register %d", registers.size());
	return VirtualHandle();
}
VirtualHandle CodeBlock::Set(Label label) {
	return VirtualHandle();
}
void CodeBlock::Get(const std::string & n, std::variant<size_t, Label, _reg, nullobj> v) {
	int nn = gets.size() + 1;
	if (std::holds_alternative<_reg>(v)) {
		auto str = std::get<_reg>(v);
		nn = str.val + 1;
	}
	gets.push_back(std::make_shared<VirtualReg>(order++, nn, n, v));
	if (std::holds_alternative<size_t>(v)) {
		auto str = std::get<size_t>(v);
		gets.back()->type = SymbolType::UnsignedInt;
	}
}
void CodeBlock::Call(const std::string & fn, std::pair<std::string, std::variant<Label, _reg, nullobj>> ret, Label & label, std::vector<std::pair<std::string, std::variant<size_t, Label, _reg, nullobj>>> args) {
	std::vector<VirtualReg*> r;
	VirtualReg* t;
	for (auto a : args) {
		if (std::holds_alternative<_reg>(a.second)) {
			auto str = std::get<_reg>(a.second);
			t = registers.at(str.val).get();
			r.push_back(t);
		}
		else if (std::holds_alternative<Label>(a.second)) {
			auto str = std::get<Label>(a.second);
			auto tt = std::make_shared<VirtualReg>(order++, registers.size() + 1, a.first, str);
			registers.push_back(std::move(tt));
			t = registers.back().get();
			r.push_back(t);
		}
		else if (std::holds_alternative<size_t>(a.second)) {
			auto str = std::get<size_t>(a.second);
			auto tt = std::make_shared<VirtualReg>(order++, registers.size() + 1, a.first, str);
			registers.push_back(std::move(tt));
			t = registers.back().get();
			r.push_back(t);
		}
	}
	auto tt = std::make_shared<VirtualReg>(order++, registers.size() + 1, fn, label, r);
	if (std::holds_alternative<_reg>(ret.second)) {
		auto str = std::get<_reg>(ret.second);
		t = registers.at(str.val).get();
		tt->ret = t;
	}
	else if (std::holds_alternative<Label>(ret.second)) {
		auto str = std::get<Label>(ret.second);
		auto t1 = std::make_shared<VirtualReg>(order++, registers.size() + 1, ret.first, str);
		gets.push_back(std::move(t1));
		t = gets.back().get();
		tt->ret = t;
	}
	registers.push_back(std::move(tt));
}
void CodeBlock::Print() {
	std::cout << '\n';
	//if (expects_return) {
		//registers.back()->id = 0;
	//}
	all.resize(registers.size() + gets.size());
	for (auto& r : registers)
		all[r->order] = { 1, r.get() };
	for (auto& r : gets)
		all[r->order] = { 0, r.get() };
	for (int i = 0; i < all.size(); ++i) {
		if (all[i].first) {
			std::string depends_on = "";
			std::string ret = "";
			if (all[i].second->ret)
				ret += all[i].second->ret->name + " = ";
			for (auto d : all[i].second->depends_on)
				depends_on += d->name + "(reg_" + std::to_string(d->id) + ")" + " ";
			if (all[i].second->type == SymbolType::Plus) {
				std::cout << ret << "reg_" << all[i].second->id << " = " << all[i-1].second->id << " + " << all[i].second->name << " " << depends_on << " " << '\n';
			} 
				std::cout << ret << "reg_" << all[i].second->id << " = " << all[i].second->name << " " << depends_on << " " << '\n';
		}
		else {
			std::string depends_on = "";
			for (auto d : all[i].second->dependant_on) {
				if (d->type == SymbolType::Plus) {
					if (std::holds_alternative<size_t>(d->value))
						depends_on += d->name + " + ";
					else
						depends_on += d->name + " + (reg_" + std::to_string(d->id) + ")" + " ";
				}
				else {
					depends_on += d->name + "(reg_" + std::to_string(d->id) + ")" + " ";

				}
			}
			//std::cout << "reg_" << all[i].second->id << " = " << all[i].second->name << " " << depends_on << " " << '\n';
			if (i == all.size() - 1) { //if last statement is a get, then return it
				if (all[i].second->type == SymbolType::NA) {
					std::cout << "reg_0" << " = " << " " << depends_on << all.back().second->name << "(reg_" + std::to_string(all.back().second->id) + ")" << " " << '\n';
				}
				else if (all[i].second->type == SymbolType::UnsignedInt) {
					std::cout << "reg_0" << " = " << depends_on << all.back().second->name << " " << '\n';
				}
			}
		}
	}
}
//expects return so move last reg into reg_0
void CodeBlock::Ret() {
	expects_return = true;
	//std::vector<VirtualReg*> args = { registers.back().get() };
	//auto t1 = std::make_shared<VirtualReg>(order++, registers.size() + 1, "ret", nullobj(), args);
	//registers.push_back(std::move(t1));
}

void CodeBlock::Optimize() {
	for (int i = 0; i < registers.size(); ++i) {
		auto& r = registers[i];
		if (r->dependant_on.size() == 0) {
			//decrement order for all things after this
			for (auto& rr : registers)
				if (rr->order > r->order) {
					rr->order--;
					rr->id--;
				}
			for (auto& rr : gets)
				if (rr->order > r->order){
					rr->order--;
					rr->id--;
				}
		}
	}
	if (expects_return) {
		registers.back()->id = 0;
	}
	registers.erase(std::remove_if(registers.begin(), registers.end(),	[](std::shared_ptr<VirtualReg> x) {return x->dependant_on.size() == 0 && x->id != 0; }), registers.end());
	std::cout << '\n';
	all.resize(registers.size() + gets.size());
	for (auto& r : registers)
		all[r->order] = { 1, r.get() };
	for (auto& r : gets)
		all[r->order] = { 0, r.get() };
	for (int i = 0; i < all.size(); ++i) {
		if (all[i].first) {
			std::string depends_on = "";
			std::string ret = "";
			if (all[i].second->ret)
				ret += all[i].second->ret->name + " = ";
			for (auto d : all[i].second->depends_on)
				depends_on += d->name + "(reg_" + std::to_string(d->id) + ")" + " ";
			std::cout << ret << "reg_" << all[i].second->id << " = " << all[i].second->name << " " << depends_on << " " << '\n';
		}
		else {
			if (i == all.size() - 1) { //if last statement is a get, then return it
				std::string depends_on = "";
				std::cout << "reg_0" << " = " << all.back().second->name << " " << depends_on << " " << '\n';

			}
		}
	}
}
