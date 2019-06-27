#pragma once
#include "mem.h"
struct MemoryPages {
	uint8_t *mem;                   // Pointer to the start of the executable memory
	size_t page_size;               // OS defined memory page size (typically 4096 bytes)
	size_t pages = 0;               // no of memory pages requested from the OS
	size_t position = 0;            // current position to the non used memory space
	size_t mem_size = 0;
	MemoryPages(size_t pages_requested = 1);

	~MemoryPages();

	// Push an uint8_t number to the memory
	void push(uint8_t data);
	// Push a function pointer to the memory
	void push_fn(size_t fn_address);
	void push_string(std::string data);
	void push_fn_h(unsigned long fn_address);

	// Push a function pointer to the memory
	void push(void(*fn)());

	// Push a vector of uint8_t numbers to the memory
	void push(const std::vector<uint8_t> &data);



	void fill(struct Label & label, const std::vector<uint8_t>& data);
	void fill_string(Label & label, const std::string & data);
	void fill(Label & label, uint8_t data);
	void fill(Label & label, uint16_t data);
	void fill(Label & label, uint32_t data);
	void fill(Label & label, uint64_t data);

	void fill(Label & label, double data);
	void fill(Label & label, float data);

	void fill(Label & label, int64_t data);

	void fill(Label & label, int32_t data);

	// Check if it there is enough available space to push some data to the memory
	void check_available_space(size_t data_size);

	// Print the content of used memory
	void show_memory();
};