#include "mem.h"
unsigned long GetPageSize() {
	SYSTEM_INFO systemInfo;
	GetSystemInfo(&systemInfo);
	return systemInfo.dwAllocationGranularity;
}

uint8_t* Alloc(uint32_t size) {
	return (uint8_t*)VirtualAlloc(0, size, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
}