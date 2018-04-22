#include "VirtualMemoryChunk.h"
#include "Windows.h"
#include <cassert>

namespace keng::memory
{
    VirtualMemoryChunk::VirtualMemoryChunk() {
        Invalidate();
    }

    VirtualMemoryChunk::VirtualMemoryChunk(size_t bytes, size_t pageSize) :
        m_pageSize(pageSize),
        m_reservedPages(BytesToPages(bytes)),
        m_data(VirtualAlloc(nullptr, m_pageSize * m_reservedPages, MEM_RESERVE, PAGE_NOACCESS)) {
        assert(m_data);
    }

    VirtualMemoryChunk::VirtualMemoryChunk(VirtualMemoryChunk&& that) {
        MoveFrom(that);
    }

    VirtualMemoryChunk::~VirtualMemoryChunk() {
        Release();
    }

    void VirtualMemoryChunk::Commit(size_t bytes) {
        assert(IsValid());
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

    void* VirtualMemoryChunk::GetData() const {
        assert(IsValid());
        return m_data;
    }

    VirtualMemoryChunk& VirtualMemoryChunk::operator=(VirtualMemoryChunk&& that) {
        Release();
        MoveFrom(that);
        return *this;
    }

    size_t VirtualMemoryChunk::BytesToPages(size_t bytes) const {
        return (bytes / m_pageSize) + 1;
    }

    void VirtualMemoryChunk::MoveFrom(VirtualMemoryChunk& that) {
        m_data = that.m_data;
        m_pageSize = that.m_pageSize;
        m_usedPages = that.m_usedPages;
        m_reservedPages = that.m_reservedPages;
        that.Invalidate();
    }

    void VirtualMemoryChunk::Release() {
        if (IsValid()) {
            VirtualFree(m_data, 0, MEM_RELEASE);
            m_data = nullptr;
            m_reservedPages = 0;
            m_usedPages = 0;
            Invalidate();
        }
    }

    void VirtualMemoryChunk::Invalidate() {
        m_data = nullptr;
    }

    bool VirtualMemoryChunk::IsValid() const {
        return m_data != nullptr;
    }
}
