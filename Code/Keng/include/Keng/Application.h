#pragma once

#include <vector>
#include <memory>

#include "Keng/Systems/ISystem.h"
#include "Keng/IApplication.h"
#include "EverydayTools/Exception/CallAndRethrow.h"
#include "EverydayTools/FrameRateCounter.h"
#include "WinWrappers/WinWrappers.h"

namespace keng {
	class Application :
		public IApplication
	{
	public:
		using Period = std::chrono::nanoseconds;
		static constexpr auto FpsSamplesCount = 100;
		static constexpr auto DesiredFPS = 60;
		using TFrameRateCounter = FrameRateCounter<Period, FpsSamplesCount>;

		virtual bool Update() override;
		virtual ISystem* AddSystem(std::unique_ptr<ISystem> system) override;
		virtual void Initialize() override;

		void Run();

		template<typename T>
		T* TryGetSystem() const {
			return CallAndRethrowM + [&]() -> T* {
				for (auto& system : m_systems) {
					if (auto ptr = dynamic_cast<T*>(system.get()))
					{
						return ptr;
					}
				}

				return nullptr;
			};
		}

		template<typename T>
		T* GetSystem() const {
			return CallAndRethrowM + [&] {
				auto ptr = TryGetSystem<T>();
				edt::ThrowIfFailed(ptr != nullptr, "Requested system not found");
				return ptr;
			};
		}

		void SetVSync(bool value);

	private:
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
