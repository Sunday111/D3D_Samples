#pragma once

#include <cassert>
#include <type_traits>
#include <EverydayTools\Pointer\IntrusivePtr.h>

class IRefCountObject {
public:
    virtual ~IRefCountObject() = default;
};

class RefCountObject : public IRefCountObject {
private:
    friend class RefCounter;
    mutable int m_refs = 0;
};

class RefCounter {
public:
    static void AddReference(const IRefCountObject* obj) {
        auto cased = static_cast<const RefCountObject*>(obj);
        ++cased->m_refs;
    }

    static int ReleaseReference(const IRefCountObject* obj) {
        auto cased = static_cast<const RefCountObject*>(obj);
        assert(cased->m_refs > 0);
        cased->m_refs -= 1;
        return cased->m_refs;
    }

    static int GetReferencesCount(const IRefCountObject* obj) {
        auto cased = static_cast<const RefCountObject*>(obj);
        return cased->m_refs;
    }
};

template<typename T>
using IntrusivePtr = edt::IntrusivePtr<T, RefCounter>;