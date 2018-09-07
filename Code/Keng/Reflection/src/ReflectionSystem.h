#pragma once

#include <vector>

#include "FwdDecl.h"
#include "Keng/Core/System.h"
#include "Keng/Reflection/IReflectionSystem.h"

namespace keng::reflection
{
	class ReflectionSystem : public core::System
	<
		IReflectionSystem, ReflectionSystem
	>
	{
	public:
		// ISystem
		virtual void OnSystemEvent(const keng::core::IApplicationPtr&, const  keng::core::SystemEvent&) override;

		// IReflectionSystem
		virtual void AddRegistrationTask(RegistrationTask task) override;

	private:
		void Initialize(const core::IApplicationPtr& app);
		void PostInitialize(const core::IApplicationPtr& app);
		void ExecuteRegistrationTasks(IScope& scope);

	private:
		std::vector<RegistrationTask> m_registrationTasks;
	};
}