#pragma once

#include <memory>

namespace keng
{
	class ISystem;

	class IApplication
	{
	public:
		virtual bool Update() = 0;
		virtual ISystem* AddSystem(ISystem* system) = 0;
		virtual void Initialize() = 0;
		virtual ~IApplication() = default;
	};
}
