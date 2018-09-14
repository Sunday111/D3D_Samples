#pragma once

#include <vector>

#include "FwdDecl.h"
#include "Keng/Core/System.h"
#include "Keng/Reflection/IReflectionSystem.h"
#include "Scope.h"

namespace keng::reflection
{
	class ReflectionSystem : public core::System
	<
		IReflectionSystem, ReflectionSystem
	>
	{
	public:
		ReflectionSystem();
		~ReflectionSystem();

		// ISystem
		virtual void OnSystemEvent(const keng::core::IApplicationPtr&, const  keng::core::SystemEvent&) override final;

		// IReflectionSystem
		virtual void AddRegistrationTask(RegistrationTask task) override final;

	private:
		void Initialize(const core::IApplicationPtr& app);
		void PostInitialize(const core::IApplicationPtr& app);
		void ExecuteRegistrationTasks(IScope& scope);

	private:
		Scope m_rootScope;
		std::vector<RegistrationTask> m_registrationTasks;
	};
}