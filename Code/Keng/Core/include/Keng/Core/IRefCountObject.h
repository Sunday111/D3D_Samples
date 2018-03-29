#pragma once

#include <type_traits>

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

    template<typename Base, typename = std::enable_if_t<std::is_base_of_v<IRefCountObject, Base>>>
    class RefCountImpl : public Base
    {
    public:
        virtual int AddReference() const override {
            return ++m_referencesCount;
        }

        virtual int RemoveReference() const override {
            auto result = --m_referencesCount;
            if (result < 1) {
                delete this;
            }
            return result;
        }

        virtual int GetReferencesCount() const override {
            return m_referencesCount;
        }

    private:
        int mutable m_referencesCount = 0;
    };
}
