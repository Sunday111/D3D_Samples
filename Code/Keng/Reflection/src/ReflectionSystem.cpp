#include "ReflectionSystem.h"
#include "Keng/Core/SystemEvent.h"
#include "Scope.h"
namespace keng::reflection
{
	static ScopeParameters MakeRootScopeParameters() {
		return ScopeParameters {
			"root",
			edt::GUID::Create("6E69BBCB-B2FC-463E-A695-89EA667E3A1F")
		};
	}

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
			ExecuteRegistrationTasks(m_rootScope);
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

	ReflectionSystem::ReflectionSystem() :
		m_rootScope(MakeRootScopeParameters())
	{
	}

	ReflectionSystem::~ReflectionSystem() = default;

}

