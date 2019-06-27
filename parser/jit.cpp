#include "std.core.h"
#include "jit.h"
#include "vm.h"

MemoryPages::MemoryPages(size_t pages_requested) {
	page_size = GetPageSize(); // Get the machine page size
	pages_requested = 100;
	mem_size = page_size * pages_requested;
	mem = (uint8_t*)Alloc(mem_size);
	if (!mem) {
		throw std::runtime_error("Can't allocate enough executable memory!");
	}
	pages = pages_requested;
}

MemoryPages::~MemoryPages() {
	//munmap(mem, pages * page_size);
}

// Push an uint8_t number to the memory
void MemoryPages::push(uint8_t data) {
	check_available_space(sizeof data);
	mem[position] = data;
	position++;
}
// Push a function pointer to the memory
void MemoryPages::push_fn(size_t fn_address) {
	check_available_space(sizeof fn_address);

	std::memcpy((mem + position), &fn_address, sizeof fn_address);
	position += sizeof fn_address;
}
void MemoryPages::push_string(std::string data) {
	data += '\0';
	check_available_space(data.size());
	std::memcpy((mem + position), data.data(), data.size());
	position += data.size();
}
void MemoryPages::push_fn_h(unsigned long fn_address) {
	check_available_space(sizeof fn_address);

	std::memcpy((mem + position), &fn_address, sizeof fn_address);
	position += sizeof fn_address;
}

// Push a function pointer to the memory
void MemoryPages::push(void(*fn)()) {
	size_t fn_address = reinterpret_cast<size_t>(fn);
	check_available_space(sizeof fn_address);

	std::memcpy((mem + position), &fn_address, sizeof fn_address);
	position += sizeof fn_address;
}

// Push a vector of uint8_t numbers to the memory
void MemoryPages::push(const std::vector<uint8_t> &data) {
	check_available_space(data.size());

	std::memcpy((mem + position), &data[0], data.size());
	position += data.size();
}
void MemoryPages::fill_string(Label& label, const std::string &data) {
	auto pos = label.ptr + label.position;
	//check_available_space(data.size());
	if (data.size() + label.position > 250)
		printf("Error: Function takes too much space, increase function space allowance or split up function.");
	std::memcpy((mem + pos), &data[0], data.size());
	label.position += data.size();
}
void MemoryPages::fill(Label& label, const std::vector<uint8_t> &data) {
	auto pos = label.ptr + label.position;
	//check_available_space(data.size());
	if (data.size() + label.position > 250)
		printf("Error: Function takes too much space, increase function space allowance or split up function.");
	std::memcpy((mem + pos), &data[0], data.size());
	label.position += data.size();
}
void MemoryPages::fill(Label& label, uint8_t data) {
	auto pos = label.ptr + label.position;
	std::memcpy((mem + pos), &data, sizeof(data));
	label.position += sizeof(data);
}
void MemoryPages::fill(Label& label, uint16_t data) {
	auto pos = label.ptr + label.position;
	std::memcpy((mem + pos), &data, sizeof(data));
	label.position += sizeof(data);
}
void MemoryPages::fill(Label& label, uint32_t data) {
	auto pos = label.ptr + label.position;
	std::memcpy((mem + pos), &data, sizeof(data));
	label.position += sizeof(data);
}
void MemoryPages::fill(Label& label, uint64_t data) {
	auto pos = label.ptr + label.position;
	std::memcpy((mem + pos), &data, sizeof(data));
	label.position += sizeof(data);
}
void MemoryPages::fill(Label& label, double data) {
	auto pos = label.ptr + label.position;
	std::memcpy((mem + pos), &data, sizeof(data));
	label.position += sizeof(data);
}
void MemoryPages::fill(Label& label, float data) {
	auto pos = label.ptr + label.position;
	std::memcpy((mem + pos), &data, sizeof(data));
	label.position += sizeof(data);
}
void MemoryPages::fill(Label& label, int64_t data) {
	auto pos = label.ptr + label.position;
	std::memcpy((mem + pos), &data, sizeof(data));
	label.position += sizeof(data);
}
void MemoryPages::fill(Label& label, int32_t data) {
	auto pos = label.ptr + label.position;
	std::memcpy((mem + pos), &data, sizeof(data));
	label.position += sizeof(data);
}

// Check if it there is enough available space to push some data to the memory
void MemoryPages::check_available_space(size_t data_size) {
	if (position + data_size > pages * page_size) {
		throw std::runtime_error("Not enough virtual memory allocated!");
	}
}

// Print the content of used memory
void MemoryPages::show_memory() {
	std::cout << "\nMemory content: " << position << "/" << pages * page_size << " bytes used\n";
	std::cout << std::hex;
	for (size_t i = 0; i < position; ++i) {
		std::cout << "" << (int)mem[i] << " ";
		if (i % 16 == 0 && i > 0) {
			std::cout << '\n';
		}
	}
	std::cout << std::dec;
	std::cout << "\n\n";
}



