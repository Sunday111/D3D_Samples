#include "MemoryManager.h"
#include <cassert>

namespace keng::memory
{
    MemoryManager::MemoryManager() :
        m8(10, 100000),
        m16(10, 100000),
        m32(10, 100000),
        m64(10, 100000)
    {
    }

    MemoryManager::~MemoryManager()
    {
    }

    void* MemoryManager::Allocate(size_t size) {
        void* result = nullptr;

             if (size <=   8) result =  m8.Allocate();
        else if (size <=  16) result = m16.Allocate();
        else if (size <=  32) result = m32.Allocate();
        else if (size <=  64) result = m64.Allocate();

        if (!result) {
            result = malloc(size);
        }

        assert(result);
        return result;
    }

    void MemoryManager::Deallocate(void* pointer) {
        assert(pointer);

        auto ok =
            m8.TryDeallocate(pointer) ||
            m16.TryDeallocate(pointer) ||
            m32.TryDeallocate(pointer) ||
            m64.TryDeallocate(pointer);

        if (!ok) {
            free(pointer);
        }
    }

    MemoryManager& MemoryManager::Instance() {
        static MemoryManager instance;
        return instance;
    }
}
