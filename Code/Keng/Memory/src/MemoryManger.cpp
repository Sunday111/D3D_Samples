#include "MemoryManager.h"
#include <cassert>

namespace keng::memory
{
    MemoryManager::MemoryManager() {
        m_pools.emplace_back( 8, 10, 100000);
        m_pools.emplace_back(16, 10, 100000);
        m_pools.emplace_back(24, 10, 100000);
        m_pools.emplace_back(32, 10, 100000);
        m_pools.emplace_back(40, 10, 100000);
        m_pools.emplace_back(48, 10, 100000);
        m_pools.emplace_back(56, 10, 100000);
        m_pools.emplace_back(64, 10, 100000);

        m_sortedBySize.reserve(m_pools.size());
        m_sortedByStartAddress.reserve(m_pools.size());
        for (auto& pool : m_pools) {
            m_sortedBySize.push_back(&pool);
            m_sortedByStartAddress.push_back(&pool);
        }

        std::sort(m_sortedBySize.begin(), m_sortedBySize.end(), [](VirtualPool* a, VirtualPool* b) {
            return a->GetElementSize() < b->GetElementSize();
        });

        std::sort(m_sortedByStartAddress.begin(), m_sortedByStartAddress.end(), [](VirtualPool* a, VirtualPool* b) {
            return a->GetBeginAddress() < b->GetBeginAddress();
        });
    }

    MemoryManager::~MemoryManager()
    {
    }

    void* MemoryManager::Allocate(size_t size) {
        auto it = std::lower_bound(
            m_sortedBySize.begin(), m_sortedBySize.end(), size,
            [](VirtualPool* b, size_t val) {
            return b->GetElementSize() < val;
        });

        void* result = nullptr;
        if (m_sortedBySize.end() != it) {
            assert((*it)->GetElementSize() >= size);
            result = (*it)->Allocate();
        }

        if (result == nullptr) {
            result = malloc(size);
        }

        return result;
    }

    void MemoryManager::Deallocate(void* pointer) {
        assert(pointer != nullptr);
        auto address = (size_t)pointer;

        auto it = std::upper_bound(
            m_sortedByStartAddress.rbegin(), m_sortedByStartAddress.rend(), address,
            [](size_t address, VirtualPool* b) {
            return address >= b->GetBeginAddress();
        });

        if (it != m_sortedByStartAddress.rend()) {
            auto& pool = **it;
            assert(pool.GetBeginAddress() <= address);
            if (pool.GetEndAddress() > address) {
                pool.Deallocate(pointer);
                return;
            }
        }

        free(pointer);
    }

    MemoryManager& MemoryManager::Instance() {
        static MemoryManager instance;
        return instance;
    }
}
