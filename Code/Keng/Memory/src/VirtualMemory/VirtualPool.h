#pragma once

#include "VirtualVector.h"
#include "EverydayTools/Bitset.h"

namespace keng::memory
{
    template<size_t bytes>
    class VirtualPool
    {
    private:
        struct Chunk
        {
            static volatile size_t csz;

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

        ~VirtualPool() {
            auto head = m_nextFree;
            edt::Bitset visited(m_vector.GetSize());

            while (head)
            {
                auto startAddress = m_vector.GetStartAddress();
                auto headAddress = (size_t)head;

                // Address does not belong to this pool
                assert(headAddress >= startAddress);

                // Invalid address
                assert(((headAddress - startAddress) % bytes) == 0);

                auto index = (headAddress - startAddress) / bytes;

                // Circle in free objects list check
                assert(!visited.IsSet(index));

                visited.Set(index);
                head = head->body.header.nextFree;
            }

            // Memory leak (or static object keeps reference...)
            assert(visited.All());
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

    private:
        Chunk& ChunckAt(size_t index) {
            return (Chunk&)m_vector[index];
        }

        VirtualVector<Chunk> m_vector;
        Chunk* m_nextFree = nullptr;
    };
}
