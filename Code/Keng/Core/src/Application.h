#pragma once

#include "Keng/Core/ISystem.h"
#include "Keng/Core/IApplication.h"

#include "EverydayTools/Exception/CallAndRethrow.h"
#include "EverydayTools/FrameRateCounter.h"

#include <chrono>
#include <vector>
#include <memory>

namespace keng::core {
    class Application :
        public IApplication
    {
    public:
        Application() = default;
        Application(const Application&) = delete;

        using Period = std::chrono::nanoseconds;
        static constexpr auto FpsSamplesCount = 100;
        static constexpr auto DesiredFPS = 60;
        using TFrameRateCounter = FrameRateCounter<Period, FpsSamplesCount>;

        // IApplication
        virtual bool Update() override;
        virtual void Initialize(const ApplicationStartupParameters& params) override;
        virtual void Run() override;
        virtual ISystem* FindSystem(std::string_view name) const override;

        void SetVSync(bool value);

    private:
        void LoadModule(std::string_view name);
        void LoadDependencies();

        bool UpdateSystems() {
            return CallAndRethrowM + [&] {
                bool finished = false;
                for (auto& system : m_systems) {
                    if (!system->Update()) {
                        finished = true;
                    }
                }
                return !finished;
            };
        }

        bool m_vSync = true;
        TFrameRateCounter m_fpsCounter;
        std::vector<std::unique_ptr<ISystem>> m_systems;
    };
}
