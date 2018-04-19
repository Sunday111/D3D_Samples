#pragma once

#include "VirtualVector.h"

namespace keng::memory
{
    template<size_t bytes>
    class VirtualPool
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
        VirtualPool(size_t resize, size_t capacity) :
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
}