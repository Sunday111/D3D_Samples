#pragma once

#include <new>
#include <map>
#include "Windows.h"
#include <cassert>
#include <vector>
#include <map>
#include <memory>

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

    template<typename T, size_t resizeValue, size_t Capacity>
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
            auto poolAddress = reinterpret_cast<size_t>(m_memory.data);
            auto pointerAddress = reinterpret_cast<size_t>(pointer);
            if (pointerAddress < poolAddress) {
                return false;
            }
            auto delta = pointerAddress - poolAddress;
            auto index = delta / sizeof(T);
            return index < m_size;
        }

        size_t GetSize() const { return m_size; }
        size_t GetStartAddress() const { return (size_t)m_memory.data; }

        bool Resize(size_t size) {
            if (size > m_size) {
                if (size > Capacity) {
                    return false;
                }
                m_memory.Commit(size * sizeof(T));
            }
            m_size = size;
            return true;
        }

    private:
        size_t m_size = 0;
        VirtualMemoryChunk<resizeValue * sizeof(T)> m_memory;
    };

    template<size_t bytes>
    union MemoryChunkData
    {
        uint8_t data[bytes];
        MemoryChunkData* nextFree;
    };

    template<size_t bytes, size_t resizeValue, size_t Capacity>
    class SmallChunkPool
    {
    public:
        using Chunk = MemoryChunkData<bytes>;
        using ChunkLayout = std::aligned_storage_t<sizeof(Chunk), sizeof(void*)>;

        void* Allocate() {
            if (m_nextFree == nullptr && (!Expand())) {
                return nullptr;
            }

            void* result = &(m_nextFree->data[0]);
            m_nextFree = m_nextFree->nextFree;
            return result;
        }

        bool TryDeallocate(void* pointer) {
            if (!m_vector.OwnsThisPointer(pointer)) {
                return false;
            }
            auto p = (Chunk*)pointer;
            p->nextFree = m_nextFree;
            m_nextFree = p;
            return true;
        }

        bool Expand() {
            auto size = m_vector.GetSize();
            if (!m_vector.Resize(size + 1)) {
                return false;
            }
            m_nextFree = (Chunk*)&m_vector[size];
            m_nextFree->nextFree = nullptr;
            return true;
        }

    private:
        Chunk& ChunckAt(size_t index) {
            return (Chunk&)m_vector[index];
        }

        VirtualVector<ChunkLayout, resizeValue, Capacity> m_vector;
        Chunk* m_nextFree = nullptr;
    };

    class MemoryManager
    {
    public:
        void* Allocate(std::size_t size);
        void Deallocate(void* pointer);
        static MemoryManager& Instance();

    private:
        SmallChunkPool< 8, 10, 100000>  m8;
        SmallChunkPool<16, 10, 100000> m16;
        SmallChunkPool<32, 10, 100000> m32;
        SmallChunkPool<64, 10, 100000> m64;

        MemoryManager() = default;

        //std::map<size_t, size_t> m_stats;
    };
}