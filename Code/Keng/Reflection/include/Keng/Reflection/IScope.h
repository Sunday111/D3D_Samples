#pragma once

namespace keng::reflection
{
	class IScope
	{
	public:
		template<typename T>
		void RegisterType();

		virtual ~IScope() = default;

	protected:
		virtual void AddElement() = 0;
	};
}