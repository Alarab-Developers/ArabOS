#ifndef MEMORY_API_H
#define MEMORY_API_H

#include <stddef.h>
#include <stdint.h>

#define MEM_PRESENT  1
#define MEM_WRITE    2
#define MEM_USER     4



typedef struct {
    // Heap
    void*   (*alloc)(size_t size);
    void    (*free)(void* ptr);

    // Paging
    uint64_t (*alloc_page)();
    void     (*free_page)(uint64_t);

    void (*map)(uint64_t virt, uint64_t phys, uint64_t flags);
    void (*map_to)(uint64_t* pml4, uint64_t virt, uint64_t phys, uint64_t flags);

    //  إصلاح #5: init تقبل fb_addr كما هو معرّف في memory_api.c
    void (*init)(void);

    uint64_t* (*get_pml4)();

    uint64_t* (*create_address_space)();
    void (*switch_address_space)(uint64_t*);

    uint64_t* (*get_current_address_space)();


} memory_api_t;

extern memory_api_t memory_api;

#endif
