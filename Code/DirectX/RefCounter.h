#pragma once

#include <cassert>
#include <type_traits>
#include <EverydayTools\Pointer\IntrusivePtr.h>

class IRefCountObject {
public:
    virtual int GetReferencesCount() const = 0;
    virtual int AddReference() const = 0;
    virtual int ReleaseReference() const = 0;
    virtual ~IRefCountObject() = default;
};

template
<
    typename T,
    typename Enable = std::enable_if_t<std::is_base_of_v<IRefCountObject, T>>
>
class RefCountImpl : public virtual T {
private:
    int GetReferencesCount() const override {
        return m_refs;
    }

    int AddReference() const override {
        return ++m_refs;
    }

    int ReleaseReference() const override {
        assert(m_refs > 0);
        return --m_refs;
    }

    friend class RefCounter;
    mutable int m_refs = 0;
};

class RefCounter {
public:
    static int AddReference(const IRefCountObject* obj) {
        return obj->AddReference();
    }

    static int ReleaseReference(const IRefCountObject* obj) {
        return obj->ReleaseReference();
    }

    static int GetReferencesCount(const IRefCountObject* obj) {
        return obj->GetReferencesCount();
    }
};

template<typename T>
using IntrusivePtr = edt::IntrusivePtr<T, RefCounter>;