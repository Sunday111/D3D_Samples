#pragma once

#include "FwdDecl.h"
#include "Keng/Core/IGlobalEnvironment.h"
#include <memory>
#include <thread>
#include <unordered_map>
#include <vector>
#include <mutex>

namespace keng::core
{
    class GlobalEnvironment : public IGlobalEnvironment
    {
    public:
        GlobalEnvironment();
        ~GlobalEnvironment();

        virtual IGlobalSystem& GetGlobalSystem(size_t id) override;
        void RegisterModule(ModulePtr module);

        static GlobalEnvironment& PrivateInstance();

        Application* CreateApplication();
        void DestroyApplication(Application*);
        ISystemPtr TryGetSystem(std::string_view name);

    private:
        std::mutex m_mutex;
        std::unordered_map<std::thread::id, ApplicationPtr> m_threadToApplication;
        std::vector<ApplicationPtr> m_applications;
        std::vector<ModulePtr> m_modules;
    };
}
