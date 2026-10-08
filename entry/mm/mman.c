#ifndef _POSIX_C_SOURCE
	#define _POSIX_C_SOURCE 200809L
#endif
#include <sys/mman.h>
#include <string.h>
#define NULL (void*)0
#define get_size(mem) *(__uint64_t*)((char*)mem - sizeof(__uint64_t))
#define get_real_memory(mem) (void*)((char*)mem - sizeof(__uint64_t))

__attribute__((hot, aligned(64), assume_aligned(16, sizeof(__uint64_t)), malloc)) void* Malloc(__uint64_t size) {
	if (__builtin_expect(size <= 0, 0)) return NULL;
	size += sizeof(__uint64_t);
	__attribute__((aligned(16))) void* allocated_memory = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
	if (__builtin_expect(allocated_memory == MAP_FAILED, 0)) return NULL;
	*((__uint64_t*)allocated_memory) = size;
	return (void*)((char*)allocated_memory + sizeof(__uint64_t));
}

__attribute__((hot, aligned(64))) void Free(void* mem) {
	if (__builtin_expect(mem == NULL, 0)) return;
	__uint64_t size = get_size(mem);
	void* real_memory = get_real_memory(mem);
	munmap(real_memory, size);
}

__attribute__((hot, aligned(64), assume_aligned(16, sizeof(__uint64_t)), malloc)) void* Realloc(void* mem, __uint64_t newSize) {
	if (__builtin_expect(mem == NULL, 0)) return NULL;
	void* newMemory = Malloc(newSize);
	if (__builtin_expect(newMemory == NULL, 0)) return NULL;
	__uint64_t beforeDataLength = get_size(mem) - sizeof(__uint64_t);
	memcpy(newMemory, mem, beforeDataLength);
	return newMemory;	
}
