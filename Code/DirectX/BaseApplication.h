#pragma once

#include <vector>
#include <memory>

#include "ISystem.h"
#include "IApplication.h"
#include "EverydayTools\Exception\CallAndRethrow.h"
#include "EverydayTools\FrameRateCounter.h"
#include "WinWrappers\WinWrappers.h"

class BaseApplication :
    public IApplication
{
public:
    using Period = std::chrono::nanoseconds;
    static constexpr auto FpsSamplesCount = 100;
    static constexpr auto DesiredFPS = 60;
    using TFrameRateCounter = FrameRateCounter<Period, FpsSamplesCount>;

    BaseApplication(bool vSync) :
        m_vSync(vSync)
    {}

    void Run() {
        CallAndRethrowM + [&]{
            while (Update());
        };
    }

    virtual bool Update() {
        return CallAndRethrowM + [&]{
            if (m_vSync) {
                using namespace std::chrono;
                auto t0 = high_resolution_clock::now();
                auto retVal = UpdateSystems();
                auto t1 = high_resolution_clock::now();
                auto average = m_fpsCounter.CalcAverageTick(t1 - t0);
                auto sleep = TFrameRateCounter::GetDesiredFrameDuration(DesiredFPS) - average;
                if (sleep.count() > 0) {
                    //std::this_thread::sleep_for(sleep);
                    Sleep(sleep);
                }
                return retVal;
            }
            else {
                return UpdateSystems();
            }
        };
    }

    ISystem* AddSystem(std::unique_ptr<ISystem> system) {
        m_systems.push_back(std::move(system));
        return m_systems.back().get();
    }

private:
    bool UpdateSystems() {
        return CallAndRethrowM + [&] {
            bool finished = false;
            for (auto& system : m_systems) {
                if (!system->Update(this)) {
                    finished = true;
                }
            }
            return !finished;
        };
    }

    std::vector<std::unique_ptr<ISystem>> m_systems;

    bool m_vSync = true;
    TFrameRateCounter m_fpsCounter;
};