#pragma once

#include "Keng/Systems/ISystem.h"
#include "EverydayTools/UnusedVar.h"
#include "EverydayTools/Exception/CallAndRethrow.h"
#include <string>
#include <vector>

namespace keng
{
	template<typename Derived>
    class System : public ISystem
    {
	public:
		virtual bool ForEachSystemDependency(bool(*pfn)(const char* systemGUID, void* context), void* context) override {
			return CallAndRethrowM + [&]() -> bool {
				for (auto& guid : m_dependencies) {
					if (pfn(guid.data(), context)) {
						return true;
					}
				}

				return false;
			};
		}

		virtual const char* GetSystemGUID() override {
			return Derived::GetGUID();
		}

	protected:
		std::vector<std::string> m_dependencies;
    };
}
