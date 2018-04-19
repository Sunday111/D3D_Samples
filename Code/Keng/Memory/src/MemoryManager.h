#pragma once

#include <new>
#include <map>
#include "Windows.h"
#include <cassert>
#include <vector>
#include <map>
#include <memory>
#include <fstream>

namespace keng::memory
{
    class VirtualMemoryChunk
    {
    public:
        VirtualMemoryChunk(size_t bytes, size_t pageSize) :
            m_pageSize(pageSize),
            m_reservedPages(BytesToPages(bytes)),
            m_data(VirtualAlloc(nullptr, m_pageSize * m_reservedPages, MEM_RESERVE, PAGE_NOACCESS))
        {
            assert(m_data);
        }

        VirtualMemoryChunk(const VirtualMemoryChunk&) = delete;

        ~VirtualMemoryChunk() {
            if (!m_data) {
                VirtualFree(m_data, 0, MEM_RELEASE);
                m_data = nullptr;
            }
            m_reservedPages = 0;
            m_usedPages = 0;
        }

        void Commit(size_t bytes) {
            auto pages = BytesToPages(bytes);
            assert(pages <= m_reservedPages);
            if (pages > m_usedPages) {
                auto startAddress = reinterpret_cast<size_t>(m_data);
                auto commitAddress = reinterpret_cast<void*>(startAddress + m_usedPages * m_pageSize);
                auto pagesToCommit = pages - m_usedPages;
                auto bytesToCommit = pagesToCommit * m_pageSize;
                VirtualAlloc(commitAddress, bytesToCommit, MEM_COMMIT, PAGE_READWRITE);
                m_usedPages = pages;
            }
        }

        void* GetData() const
        {
            assert(m_usedPages > 0);
            return m_data;
        }

    protected:
        size_t BytesToPages(size_t bytes) const {
            return (bytes / m_pageSize) + 1;
        }

    private:
        size_t m_pageSize = 100;
        size_t m_usedPages = 0;
        size_t m_reservedPages = 0;
        void* m_data = nullptr;
    };

    template<typename T>
    class VirtualVector
    {
    public:
        VirtualVector(size_t capacity, size_t resizeValue = 1) :
            m_resizeValue(resizeValue),
            m_capacity(capacity),
            m_memory(m_capacity * sizeof(T), m_resizeValue * sizeof(T))
        {}

        T& operator[](size_t index) {
            assert(index < m_size);
            return ((T*)m_memory.GetData())[index];
        }

        bool OwnsThisPointer(void* pointer) const {
            auto poolAddress = reinterpret_cast<size_t>(m_memory.GetData());
            auto pointerAddress = reinterpret_cast<size_t>(pointer);
            if (pointerAddress < poolAddress) {
                return false;
            }
            auto delta = (pointerAddress - poolAddress);
            auto index = delta / sizeof(T);
            return index < m_size;
        }

        size_t GetSize() const { return m_size; }
        size_t GetStartAddress() const { return (size_t)m_memory.GetData(); }

        bool Resize(size_t size) {
            if (size > m_size) {
                if (size > m_capacity) {
                    return false;
                }
                m_memory.Commit(size * sizeof(T));
            }
            m_size = size;
            return true;
        }

    private:
        size_t m_size = 0;
        size_t m_resizeValue = 1;
        size_t m_capacity;
        VirtualMemoryChunk m_memory;
    };

    template<size_t bytes>
    class SmallChunkPool
    {
    private:
        struct Chunk
        {
            struct Header
            {
                Chunk* nextFree;
            };

            struct Data
            {
                uint8_t mem[bytes];
            };

            union Body
            {
                Header header;
                Data data;
            };

            Body body;
        };
    public:
        SmallChunkPool(size_t resize, size_t capacity) :
            m_vector(capacity, resize)
        {
        }

        void* Allocate() {
            if (m_nextFree == nullptr && (!Expand())) {
                return nullptr;
            }

            void* result = &(m_nextFree->body.data.mem[0]);
            m_nextFree = m_nextFree->body.header.nextFree;
            return result;
        }

        bool TryDeallocate(void* pointer) {
            if (!m_vector.OwnsThisPointer(pointer)) {
                return false;
            }
            auto p = (Chunk*)pointer;
            p->body.header.nextFree = m_nextFree;
            m_nextFree = p;
            return true;
        }

        bool Expand() {
            auto size = m_vector.GetSize();
            if (!m_vector.Resize(size + 1)) {
                return false;
            }
            m_nextFree = (Chunk*)&m_vector[size];
            m_nextFree->body.header.nextFree = nullptr;
            return true;
        }

        constexpr size_t GetElementSize() const { return bytes; }

    private:
        Chunk& ChunckAt(size_t index) {
            return (Chunk&)m_vector[index];
        }

        VirtualVector<Chunk> m_vector;
        Chunk* m_nextFree = nullptr;
    };

    class MemoryManager
    {
    public:
        void* Allocate(std::size_t size);
        void Deallocate(void* pointer);
        static MemoryManager& Instance();

    private:
        MemoryManager();
        ~MemoryManager();

        SmallChunkPool< 8>  m8;
        SmallChunkPool<16> m16;
        SmallChunkPool<32> m32;
        SmallChunkPool<64> m64;
    };
}