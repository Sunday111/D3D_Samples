#include "MemoryManager.h"

namespace keng::memory
{
    void* MemoryManager::Allocate(size_t size) {
        switch (size)
        {
        case  8: return  m8.Allocate();
        case 16: return m16.Allocate();
        case 32: return m32.Allocate();
        case 64: return m64.Allocate();
        }

        auto result = malloc(size);
        assert(result);
        return result;
    }

    void MemoryManager::Deallocate(void* pointer) {
        if (m8.TryDeallocate(pointer)) return;
        if (m16.TryDeallocate(pointer)) return;
        if (m32.TryDeallocate(pointer)) return;
        if (m64.TryDeallocate(pointer)) return;

        assert(pointer);
        free(pointer);
    }

    MemoryManager& MemoryManager::Instance() {
        static MemoryManager instance;
        return instance;
    }
}
