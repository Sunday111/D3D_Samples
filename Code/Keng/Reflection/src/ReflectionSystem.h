#pragma once

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

	private:
		void Initialize(const core::IApplicationPtr& app);
	};
}