#pragma once

namespace keng::core
{
    class IRefCountObject
    {
    public:
        virtual int AddReference() const = 0;
        virtual int RemoveReference() const = 0;
        virtual int GetReferencesCount() const = 0;
        virtual ~IRefCountObject() = default;
    };
}

#define IMPLEMENT_IREFCOUNT                                 \
    public:                                                 \
        virtual int AddReference() const override {         \
            return ++m_referencesCount;                     \
        }                                                   \
        virtual int RemoveReference() const override {      \
            return --m_referencesCount;                     \
        }                                                   \
        virtual int GetReferencesCount() const override {   \
            return m_referencesCount;                       \
        }                                                   \
    private:                                                \
        int mutable m_referencesCount = 0;
