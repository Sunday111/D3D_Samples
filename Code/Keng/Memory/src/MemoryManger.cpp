#include "MemoryManager.h"
#include <new>
#include <cassert>

namespace keng::memory
{
    void* MemoryManager::Allocate(size_t size) {
        auto result = malloc(size);
        assert(result);
        return result;
    }

    void MemoryManager::Deallocate(void* pointer) {
        assert(pointer);
        free(pointer);
    }

    MemoryManager& MemoryManager::Instance() {
        static MemoryManager instance;
        return instance;
    }
}
