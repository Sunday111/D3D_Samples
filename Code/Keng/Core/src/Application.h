#pragma once

#include "FwdDecl.h"
#include "Keng/Core/IApplication.h"
#include "Keng/Core/ISystem.h"
#include "Module.h"
#include "EverydayTools/FrameRateCounter.h"
#include <chrono>
#include <vector>
#include <memory>

namespace keng::core {
    class Application : public RefCountImpl<IApplication>
    {
    public:
        Application();
        Application(const Application&) = delete;

        using Period = std::chrono::nanoseconds;

        // IApplication
        virtual void Update() override;
        virtual void Initialize(const ApplicationStartupParameters& params) override;
        virtual void Run() override;
        virtual void Shutdown() override;
        virtual ISystemPtr FindSystem(const char* name) const override;
        virtual void SetVSync(bool value) override;

    private:
        void LoadModule(std::string_view name);
        void LoadDependencies();
        void UpdateSystems();
		void FreeModules();
		void NotifyAll(const SystemEvent&);
		void NotifyAllReversed(const SystemEvent&);

    private:
        bool m_vSync = false;
        bool m_quiting = false;
        FrameRateCounter<Period> m_fpsCounter;
        std::vector<ModulePtr> m_modules;
    };
}
