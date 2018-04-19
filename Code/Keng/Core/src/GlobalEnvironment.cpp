#include "GlobalEnvironment.h"
#include "EverydayTools/Exception/CallAndRethrow.h"
#include "EverydayTools/Exception/ThrowIfFailed.h"
#include "Keng/Core/IGlobalSystem.h"
#include <memory>

namespace keng::core
{
    namespace
    {
        static std::unique_ptr<GlobalEnvironment> globalInstance;
    }

    IGlobalSystem& GlobalEnvironment::GetGlobalSystem(size_t id) {
        return CallAndRethrowM + [&] () -> IGlobalSystem& {
            edt::ThrowIfFailed(id < m_systems.size());
            return *m_systems[id];
        };
    }

    void GlobalEnvironment::RegisterSystem(IGlobalSystemPtr system) {
        CallAndRethrowM + [&] {
            system->AssignId(m_systems.size());
            m_systems.push_back(system);
        };
    }

    GlobalEnvironment& GlobalEnvironment::PrivateInstance() {
        return *globalInstance;
    }

    void GlobalEnvironment::Destroy() {
        globalInstance = nullptr;
    }

    void GlobalEnvironment::Initialize() {
        globalInstance = std::make_unique<GlobalEnvironment>();
    }

    IGlobalEnvironment& IGlobalEnvironment::Instance() {
        return GlobalEnvironment::PrivateInstance();
    }


}