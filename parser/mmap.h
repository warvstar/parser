#pragma once
#define MMAP_CLEANUP		// Dan

#include <io.h>
#include <windows.h>
#include <sys/types.h>

#ifdef MMAP_CLEANUP
struct mmap_cleanup_tag;
typedef struct mmap_cleanup_tag
{
	void *addr;
	HANDLE h;
	struct mmap_cleanup_tag *next;
} mmap_cleanup_t;
static mmap_cleanup_t *mmap_cleanup = NULL;
#endif


#define PROT_READ     0x1
#define PROT_WRITE    0x2
/* This flag is only available in WinXP+ */
#ifdef FILE_MAP_EXECUTE
#define PROT_EXEC     0x4
#else
#define PROT_EXEC        0x0
#define FILE_MAP_EXECUTE 0
#endif

#define MAP_SHARED    0x01
#define MAP_PRIVATE   0x02
#define MAP_ANONYMOUS 0x20
#define MAP_ANON      MAP_ANONYMOUS
#define MAP_FAILED    ((void *) -1)

#ifdef __USE_FILE_OFFSET64
# define MM_DWORD_HI(x) (x >> 32)
# define MM_DWORD_LO(x) ((x) & 0xffffffff)
#else
# define MM_DWORD_HI(x) (0)
# define MM_DWORD_LO(x) (x)
#endif

static void *mmap(void *start, size_t length, int prot, int flags, int fd, off_t offset)
{
	if (prot & ~(PROT_READ | PROT_WRITE | PROT_EXEC))
		return MAP_FAILED;
	if (fd == -1) {
		if (!(flags & MAP_ANON) || offset)
			return MAP_FAILED;
	}
	else if (flags & MAP_ANON)
		return MAP_FAILED;

	DWORD flProtect;
	if (prot & PROT_WRITE) {
		if (prot & PROT_EXEC)
			flProtect = PAGE_EXECUTE_READWRITE;
		else
			flProtect = PAGE_READWRITE;
	}
	else if (prot & PROT_EXEC) {
		if (prot & PROT_READ)
			flProtect = PAGE_EXECUTE_READ;
		else if (prot & PROT_EXEC)
			flProtect = PAGE_EXECUTE;
	}
	else
		flProtect = PAGE_READONLY;

	off_t end = (off_t)(length + offset);
	HANDLE mmap_fd, h;
	if (fd == -1)
		mmap_fd = INVALID_HANDLE_VALUE;
	else
		mmap_fd = (HANDLE)_get_osfhandle(fd);
	h = CreateFileMapping(mmap_fd, NULL, flProtect, MM_DWORD_HI(end), MM_DWORD_LO(end), NULL);
	if (h == NULL)
		return MAP_FAILED;

	DWORD dwDesiredAccess;
	if (prot & PROT_WRITE)
		dwDesiredAccess = FILE_MAP_WRITE;
	else
		dwDesiredAccess = FILE_MAP_READ;
	if (prot & PROT_EXEC)
		dwDesiredAccess |= FILE_MAP_EXECUTE;
	if (flags & MAP_PRIVATE)
		dwDesiredAccess |= FILE_MAP_COPY;
	void *ret = MapViewOfFile(h, dwDesiredAccess, MM_DWORD_HI(offset), MM_DWORD_LO(offset), length);
	if (ret == NULL) {
		CloseHandle(h);
		ret = MAP_FAILED;
	}
#ifdef MMAP_CLEANUP
	else
	{
		// Add a tracking element (to the start of our list)
		mmap_cleanup_t *mc = (mmap_cleanup_t *)malloc(sizeof(mmap_cleanup_t));
		if (mc != NULL)
		{
			mc->addr = ret;
			mc->h = h;
			mc->next = mmap_cleanup;
			mmap_cleanup = mc;
		}
	}
#endif
	return ret;
}

static void munmap(void *addr, size_t length)
{
	UnmapViewOfFile(addr);
#ifdef MMAP_CLEANUP
	// Look up through the tracking elements to close the handle
	mmap_cleanup_t **prevPtr = &mmap_cleanup;
	mmap_cleanup_t *mc;
	for (mc = *prevPtr; mc != NULL; prevPtr = &mc->next, mc = *prevPtr)
	{
		if (mc->addr == addr)
		{
			CloseHandle(mc->h);
			*prevPtr = mc->next;
			break;
		}
	}
#else
	// ruh-ro, we leaked handle from CreateFileMapping() ...
#endif
}

#define _SC_PAGE_SIZE			1
#define _SC_NPROCESSORS_CONF	2
#define _SC_NPROCESSORS_ONLN	3
#define getpagesize() sysconf(_SC_PAGE_SIZE)
#define get_nprocs_conf() sysconf(_SC_NPROCESSORS_CONF)
#define get_nprocs() sysconf(_SC_NPROCESSORS_ONLN)
static long sysconf(int name)
{
	switch (name)
	{
	case _SC_PAGE_SIZE:
	{
		SYSTEM_INFO systemInfo;
		GetSystemInfo(&systemInfo);
		return systemInfo.dwAllocationGranularity; // NOT systemInfo.dwPageSize;
	}
	case _SC_NPROCESSORS_CONF:
	case _SC_NPROCESSORS_ONLN:
	{
		SYSTEM_INFO systemInfo;
		GetSystemInfo(&systemInfo);
		return systemInfo.dwNumberOfProcessors;
	}
	default:
		return -1;
	}
}