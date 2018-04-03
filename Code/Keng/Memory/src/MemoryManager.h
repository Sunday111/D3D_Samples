#pragma once

#include <new>
#include <map>
#include "Windows.h"
#include <cassert>
#include <vector>

namespace keng::memory
{
    template<size_t pageSize = 80>
    struct VirtualMemoryChunk
    {
        VirtualMemoryChunk(size_t bytes) {
            pagesCount = BytesToPages(bytes);
            data = VirtualAlloc(nullptr, pageSize * pagesCount, MEM_RESERVE, PAGE_NOACCESS);
        }

        ~VirtualMemoryChunk() {
            if (!data) {
                VirtualFree(data, 0, MEM_RELEASE);
                data = nullptr;
            }
            pagesCount = 0;
            commitedPages = 0;
        }

        void Commit(size_t bytes) {
            assert(bytes < pageSize * pagesCount);
            auto pages = BytesToPages(bytes);
            if (pages > commitedPages) {
                VirtualAlloc(data, pages * pageSize, MEM_COMMIT, PAGE_READWRITE);
                commitedPages = pages;
            }
        }

        static size_t BytesToPages(size_t bytes) {
            return (bytes / pageSize) + 1;
        }

        size_t pagesCount = 0;
        size_t commitedPages = 0;
        void* data = nullptr;
    };

    template<typename T, size_t pageSize, size_t Capacity>
    class VirtualVector
    {
    public:
        VirtualVector() :
            m_memory(Capacity * sizeof(T))
        {}

        T& operator[](size_t index) {
            assert(index < m_size);
            return ((T*)m_memory.data)[index];
        }

        bool OwnsThisPointer(void* pointer) const {
            auto data = reinterpret_cast<size_t>(m_memory.data);
            auto address = reinterpret_cast<size_t>(pointer);
            return ((address >= data) && (address < (data + pageSize * m_memory.commitedPages)));
        }

        size_t GetSize() const { return m_size; }
        size_t GetStartAddress() const { return (size_t)m_memory.data; }

        void Resize(size_t size) {
            if (size > m_size) {
                m_memory.Commit(size * sizeof(T));
            }
            m_size = size;
        }

    private:
        size_t m_size = 0;
        VirtualMemoryChunk<pageSize> m_memory;
    };

    template<size_t bytes>
    class AllocatedMemoryChunk
    {
    public:
        uint8_t data[bytes];
        int poolIndex;
    };

    template<size_t bytes, size_t pageSize, size_t Capacity>
    class SmallChunkPool
    {
    public:
        void* Allocate() {
            auto size = m_vector.GetSize();
            for (size_t i = m_lastFree; i < Capacity && i < size; ++i) {
                if (m_vector[i].poolIndex < 0) {
                    m_vector[i].poolIndex = static_cast<int>(i);
                    m_lastFree = i;
                    return &(m_vector[i].data[0]);
                }
            }
            m_lastFree = size;
            m_vector.Resize(size + 1);
            m_vector[size].poolIndex = static_cast<int>(size);
            return &(m_vector[size].data[0]);
        }

        bool TryDeallocate(void* pointer) {
            if (m_vector.OwnsThisPointer(pointer)) {
                auto a = m_vector.GetStartAddress();
                auto b = (size_t)pointer;
                auto i = (b - a) / bytes;
                m_vector[i].poolIndex = -1;
                m_lastFree = i;
                return true;
            }

            return false;
        }

        size_t m_lastFree = 0;
        VirtualVector<AllocatedMemoryChunk<bytes>, pageSize, Capacity> m_vector;
    };

    class MemoryManager
    {
    public:
        void* Allocate(std::size_t size);
        void Deallocate(void* pointer);
        static MemoryManager& Instance();

    private:
        std::map<size_t, size_t> m_sizeToCount;

        SmallChunkPool< 8, 80, 1000>  m8;
        SmallChunkPool<16, 80, 1000> m16;
        SmallChunkPool<32, 80, 1000> m32;
        SmallChunkPool<64, 80, 1000> m64;

        MemoryManager() = default;
    };
}