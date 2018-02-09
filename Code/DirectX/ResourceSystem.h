#pragma once

#include <chrono>
#include <memory>
#include <unordered_map>
#include "RefCounter.h"
#include "ISystem.h"
#include "EverydayTools/UnusedVar.h"

namespace resource_system_details {
    namespace chr = std::chrono;
    using Clock = chr::high_resolution_clock;
    using TimePoint = chr::time_point<Clock>;
}

class IResource : public IRefCountObject {
public:
    virtual void ReadFromFile(const char* filename) = 0;
    virtual const char* GetFilename() const = 0;
    virtual int GetInvalidateSeconds() const = 0;
    virtual ~IResource() = default;
};

class ResourceSystem : public ISystem {
    using TimePoint = resource_system_details::TimePoint;

    struct ResourceInfo {
        ResourceInfo(IntrusivePtr<IResource> resource, TimePoint now) :
            ptr(std::move(resource)),
            lastRenew(now)
        {
        }

        bool NeedDiscard(const TimePoint& now) const {
            return SingleReference() && Expired(now);
        }

        bool SingleReference() const {
            return RefCounter::GetReferencesCount(ptr.Get()) <= 1;
        }

        bool Expired(const TimePoint& now) const {
            auto dt_seconds = std::chrono::duration_cast<std::chrono::seconds>(now - lastRenew);
            return dt_seconds.count() >= ptr->GetInvalidateSeconds();
        }

        void Renew(const TimePoint& now) {
            lastRenew = now;
        }

        IntrusivePtr<IResource> ptr;
        TimePoint lastRenew;
    };

public:
    ResourceSystem() :
        m_prevUpdate(CurrentTime())
    {}

    IntrusivePtr<IResource> Find(const char* filename) {
        m_findStringCache = filename;
        auto it = m_resources.find(m_findStringCache);
        if (it == m_resources.end()) {
            return nullptr;
        }
        return it->second.ptr;
    }

    bool Update(IApplication* app) override {
        UnusedVar(app);
        ReleaseExpiredObjects();
        return true;
    }

    template<typename T>
    IntrusivePtr<T> GetResource(const char* filename) {
        return CallAndRethrowM + [&] {
            {
                auto loaded = Find(filename);
                if (loaded != nullptr) {
                    return edt::DynamicPointerCast<T>(loaded);
                }
            }

            auto ptr = IntrusivePtr<T>::MakeInstance();
            ptr->ReadFromFile(filename);
            m_resources.emplace(std::make_pair(std::string(filename), ResourceInfo(ptr, CurrentTime())));
            return ptr;
        };
    }

private:
    void ReleaseExpiredObjects() {
        auto it = m_resources.begin();
        auto now = CurrentTime();

        while (it != m_resources.end()) {
            if (it->second.NeedDiscard(now)) {
                it = m_resources.erase(it);
            } else {
                it->second.Renew(now);
                ++it;
            }
        }
    }

    static TimePoint CurrentTime() {
        return std::chrono::high_resolution_clock::now();
    }

private:
    TimePoint m_prevUpdate;
    std::string m_findStringCache;
    std::unordered_map<std::string, ResourceInfo> m_resources;
};