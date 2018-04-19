#pragma once

#include "Keng/Core/IGlobalEnvironment.h"
#include <vector>

namespace keng::core
{
    class GlobalEnvironment : public IGlobalEnvironment
    {
    public:
        virtual IGlobalSystem& GetGlobalSystem(size_t id) override;
        void RegisterSystem(IGlobalSystemPtr system);
        static GlobalEnvironment& PrivateInstance();
        static void Destroy();
        static void Initialize();

    private:
        std::vector<IGlobalSystemPtr> m_systems;
    };
}
