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

    private:
        std::vector<IGlobalSystemPtr> m_systems;
    };
}
