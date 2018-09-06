#include "ReflectionSystem.h"
#include "Keng/Core/SystemEvent.h"

namespace keng::reflection
{
	void ReflectionSystem::Initialize(const core::IApplicationPtr& app)
	{
		CallAndRethrowM + [&] {
			StoreDependencies(*app);
		};
	}

	void ReflectionSystem::OnSystemEvent(const keng::core::IApplicationPtr& app, const keng::core::SystemEvent& e)
	{
		CallAndRethrowM + [&] {
			switch (e)
			{
			case core::SystemEvent::Initialize:
				Initialize(app);
				break;
			}
		};
	}

}

