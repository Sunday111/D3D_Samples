#include "EverydayTools/Exception/CallAndRethrow.h"
#include "EverydayTools/Exception/ThrowIfFailed.h"
#include "Keng/Core/Application.h"
#include <algorithm>

namespace keng
{

	void Application::SetVSync(bool value)
	{
		m_vSync = value;
	}

	void Application::Initialize()
	{
		std::vector<ISystem*> walkQueue;
		std::vector<ISystem*> initQueue;

		for (auto& sys : m_systems) {
			initQueue.push_back(sys.get());
		}

		std::stable_sort(initQueue.begin(), initQueue.end(), [&](ISystem* a, ISystem* b) {
			walkQueue.clear();

			auto a_guid = std::string_view(a->GetSystemGUID());
			bool dependencyFound = false;
			walkQueue.push_back(b);
			while (!dependencyFound && !walkQueue.empty()) {
				auto current = walkQueue.back();
				walkQueue.pop_back();
				dependencyFound = current->ForEachSystemDependency([&](const char* guid) {
					if (a_guid == std::string_view(guid)) {
						return true;
					}
					
					auto sysIt = std::find_if(m_systems.begin(), m_systems.end(), [guid](const std::unique_ptr<ISystem>& sys) {
						return std::string_view(sys->GetSystemGUID()) == std::string_view(guid);
					});
					edt::ThrowIfFailed(sysIt != m_systems.end(), "Found dependency that was not registered as system");
					walkQueue.push_back(sysIt->get());
					return false;
				});
			}

			return dependencyFound;
		});

		for (auto& system : initQueue) {
			system->Initialize(this);
		}
	}

	ISystem* Application::AddSystem(ISystem* pSystem) {
        std::unique_ptr<ISystem> system;
        system.reset(pSystem);
		m_systems.push_back(std::move(system));
		return m_systems.back().get();
	}

	bool Application::Update() {
		return CallAndRethrowM + [&] {
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

	void Application::Run() {
		CallAndRethrowM + [&] {
			while (Update());
		};
	}
}
