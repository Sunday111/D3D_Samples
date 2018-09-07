#include "ReflectionSystem.h"
#include "Keng/Core/SystemEvent.h"

namespace keng::reflection
{
	void ReflectionSystem::Initialize(const core::IApplicationPtr& app) {
		CallAndRethrowM + [&] {
			StoreDependencies(*app);
		};
	}

	void ReflectionSystem::OnSystemEvent(const keng::core::IApplicationPtr& app, const keng::core::SystemEvent& e) {
		CallAndRethrowM + [&] {
			switch (e) {
			case core::SystemEvent::Initialize:
				Initialize(app);
				break;
			case core::SystemEvent::PostInitialize:
				PostInitialize(app);
				break;
			}
		};
	}

	void ReflectionSystem::PostInitialize(const core::IApplicationPtr&) {
		CallAndRethrowM + [&] {
			IScope* scope = nullptr;
			ExecuteRegistrationTasks(*scope);
		};
	}

	void ReflectionSystem::AddRegistrationTask(RegistrationTask task) {
		CallAndRethrowM + [&] {
			m_registrationTasks.push_back(task);
		};
	}

	void ReflectionSystem::ExecuteRegistrationTasks(IScope& scope) {
		CallAndRethrowM + [&] {
			for (auto& task: m_registrationTasks) {
				task(scope);
			}
		};
	}

}

