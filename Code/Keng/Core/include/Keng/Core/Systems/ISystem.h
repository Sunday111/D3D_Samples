#pragma once

namespace keng
{
	class IApplication;

	class ISystem {
	public:
		virtual void Initialize(IApplication*) = 0;
		virtual bool Update() = 0;
		virtual const char* GetSystemGUID() = 0;
		virtual bool ForEachSystemDependency(bool(*pfn)(const char* systemGUID, void* context), void* context) = 0;
		virtual ~ISystem() = default;

		template<typename Functor>
		bool ForEachSystemDependency(Functor&& fn) {
			return CallAndRethrowM + [&] {
				return ForEachSystemDependency([](const char* systemGUID, void* context) -> bool {
					return (*((Functor*)context))(systemGUID);
				}, &fn);
			};
		}
	};
}
