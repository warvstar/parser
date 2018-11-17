#pragma once
#include "std.core.h"
#include <Windows.h>
#define _SC_PAGE_SIZE	1
unsigned long GetPageSize();

uint8_t* Alloc(uint32_t size);