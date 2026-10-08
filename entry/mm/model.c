#include "mman.h"
#include "../../base/structure.h"
#include "config.h"
#include "../../basic.h"

#define meta_of(mem) *(__uint64_t*)((char*)mem + 1)
#define get_allocated_memory(mem) (void*)((char*)mem + sizeof(_Bool) + (sizeof(__uint64_t) * 2))
#define container_of(type, mem) (type*)((char*)mem - offsetof(type, mem))
#define state_of(mem) *(_Bool*)((char*)mem - ((sizeof(__uint64_t) * 2) + sizeof(_Bool)))
#define max_of(mem) *(__uint64_t*)((char*)mem + sizeof(__uint64_t) + sizeof(_Bool))
#define max_of_am(mem) *(__uint64_t*)((char*)mem - sizeof(__uint64_t))
#define size_of(mem) *(__uint64_t*)((char*)mem - (sizeof(__uint64_t) * 2))
#define get_raw_memory(mem) (void*)((char*)mem - ((sizeof(__uint64_t) * 2) + sizeof(_Bool)))


__attribute__((aligned(64), section(".data"))) struct memory* memAllocs = NULL;



void __attribute__((constructor, no_reorder)) initMm() {
    memory* tmp = Malloc(sizeof(memory));
    if (__builtin_expect(tmp == NULL, 0)) {
        exit_program(-1)
    }
    void* initalMemory = Malloc(INITIAL_MEM_ALLOCATION_SIZE);
    if (__builtin_expect(initalMemory == NULL, 0)) {
        Free(tmp);
        exit_program(-1)
    }
    memAllocs = tmp;
    memAllocs->mem = initalMemory;
    memAllocs->base = initalMemory;
    memAllocs->capacity = INITIAL_MEM_ALLOCATION_SIZE;
    memAllocs->next = NULL;
    memAllocs->index = 0;
}


void* __attribute__((alloc_size(1), malloc, hot, aligned(64))) bMalloc(__uint64_t size) {
    if (__builtin_expect(size == 0, 0)) return NULL;
    memory* mem = memAllocs;
    __uint64_t totalSize = size + (sizeof(__uint64_t) * 2) + sizeof(_Bool);
    while (mem != NULL) {
        if (__builtin_expect(mem->capacity <= mem->index, 0)) {
            mem = mem->next;
            continue;
        }
        void* newMemory = mem->mem + mem->index;
        *(_Bool*)newMemory = 1;
        meta_of(newMemory) = size;
        mem->index += totalSize;
        __uint64_t maxMemory = mem->capacity + (__uint64_t)mem->base;
        max_of(newMemory) = maxMemory;
        return get_allocated_memory(newMemory);
    }
    __uint64_t newSize = (memAllocs->capacity * 3 ) / 2;
    memory* newMemAlloc = Malloc(sizeof(memory));
    if(__builtin_expect(newMemAlloc == NULL, 0)) return NULL;
    void* newMemorySlot = Malloc(newSize);
    if (__builtin_expect(newMemorySlot == NULL, 0)) {
        Free(newMemAlloc);
        return NULL;
    }
    memory* before = memAllocs->next;
    newMemAlloc->mem = newMemorySlot;
    newMemAlloc->base = newMemorySlot;
    newMemAlloc->capacity = newSize;
    *(_Bool*)newMemorySlot = 1;
    meta_of(newMemorySlot) = totalSize;
    __uint64_t maxMemory = newMemAlloc->capacity + (__uint64_t)newMemAlloc->base;
    max_of(newMemorySlot) = maxMemory;
    newMemAlloc->index = newSize;
    memAllocs = newMemAlloc;
    memAllocs->next = before;
    return get_allocated_memory(newMemAlloc);
}

void __attribute__((hot, aligned(64))) bFree(void* memory) {
    state_of(memory) = 0;
}

 
void* __attribute__((alloc_size(2), malloc, hot, aligned(64), nonull(1))) bRealloc(void* beforeMemory, __uint64_t newSize) {
    if (__builtin_expect(beforeMemory == NULL, 0)) return NULL;
    void* mem = beforeMemory;
    __uint64_t foundBytes = 0;
    _Bool canExpand = 1;
    while ((__uint64_t)get_raw_memory(mem) < max_of_am(beforeMemory)) {
        if (__builtin_expect(state_of(mem) == 0, 0)) {
            __uint64_t size = size_of(mem);
            foundBytes += size;
        } else {
            foundBytes = 0;
            canExpand = 0;
        }
        mem = size_of(mem) + (sizeof(__uint64_t) * 2) + sizeof(_Bool);
    }
    if (__builtin_expect(canExpand && foundBytes >= newSize, 0)) {
        
        return beforeMemory;
    }
}