#include "GlobalEnvironment.h"
#include "EverydayTools/Exception/CallAndRethrow.h"
#include "EverydayTools/Exception/ThrowIfFailed.h"
#include "Keng/Core/IGlobalSystem.h"

namespace keng::core
{
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
        static GlobalEnvironment instance;
        return instance;
    }

    IGlobalEnvironment& IGlobalEnvironment::Instance() {
        return GlobalEnvironment::PrivateInstance();
    }


}