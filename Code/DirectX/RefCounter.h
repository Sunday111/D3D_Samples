#pragma once

#include <cassert>
#include <type_traits>
#include <EverydayTools\Pointer\IntrusivePtr.h>

class RefCountObject {
public:

    virtual ~RefCountObject() = default;

private:
    friend class RefCounter;
    mutable int m_refs = 0;
};

class RefCounter {
public:
    static void AddReference(const RefCountObject* obj) {
        ++obj->m_refs;
    }

    static int ReleaseReference(const RefCountObject* obj) {
        assert(obj->m_refs > 0);
        obj->m_refs -= 1;
        return obj->m_refs;
    }

    static int GetReferencesCount(const RefCountObject* obj) {
        return obj->m_refs;
    }
};

template<typename T>
using IntrusivePtr = edt::IntrusivePtr<T, RefCounter>;