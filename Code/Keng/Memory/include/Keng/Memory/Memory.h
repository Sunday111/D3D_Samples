#pragma once

#include <new>
#include "Keng/Memory/Common.h"

namespace keng::memory
{
    class KENG_MEMORY_API Api
    {
    public:
        static void* Allocate(std::size_t size);
        static void Deallocate(void* pointer);
    };
}

#define USE_KENG_MEMORY_MANAGER                 \
    void* operator new(std::size_t n) {         \
        return keng::memory::Api::Allocate(n);  \
    }                                           \
                                                \
    void operator delete(void * p) {            \
        keng::memory::Api::Deallocate(p);       \
    }                                           \
                                                \
    void *operator new[](std::size_t n) {       \
        return keng::memory::Api::Allocate(n);  \
    }                                           \
                                                \
    void operator delete[](void *p) {           \
        keng::memory::Api::Deallocate(p);       \
    }
