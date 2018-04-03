#pragma once

#include "Keng/Core/IApplication.h"
#include "Keng/Core/ISystem.h"
#include "EverydayTools/FrameRateCounter.h"
#include "EverydayTools/Exception/CallAndRethrow.h"
#include <chrono>
#include <vector>
#include <memory>

namespace keng::core {
    class Application : public RefCountImpl<IApplication>
    {
    public:
        Application() = default;
        Application(const Application&) = delete;
        ~Application();

        using Period = std::chrono::nanoseconds;
        static constexpr auto FpsSamplesCount = 100;
        static constexpr auto DesiredFPS = 60;
        using TFrameRateCounter = FrameRateCounter<Period, FpsSamplesCount>;

        // IApplication
        virtual bool Update() override;
        virtual void Initialize(const ApplicationStartupParameters& params) override;
        virtual void Run() override;
        virtual ISystemPtr FindSystem(std::string_view name) const override;

        void SetVSync(bool value);

    private:
        void LoadModule(std::string_view name);
        void LoadDependencies();
        bool UpdateSystems();

    private:
        bool m_vSync = true;
        TFrameRateCounter m_fpsCounter;
        std::vector<ISystemPtr> m_systems;
    };
}
