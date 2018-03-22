#pragma once

#include "Keng/Core/Common.h"
#include "Keng/Core/ISystem.h"
#include "Keng/Core/IApplication.h"

#include "EverydayTools/Exception/CallAndRethrow.h"
#include "EverydayTools/FrameRateCounter.h"

#include <chrono>
#include <vector>
#include <memory>

namespace keng::core {
	class KENG_CORE_API Application :
		public IApplication
	{
	public:
        Application() = default;
        Application(const Application&) = delete;

		using Period = std::chrono::nanoseconds;
		static constexpr auto FpsSamplesCount = 100;
		static constexpr auto DesiredFPS = 60;
		using TFrameRateCounter = FrameRateCounter<Period, FpsSamplesCount>;

		virtual bool Update() override;
		virtual ISystem* AddSystem(ISystem* system) override;
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
