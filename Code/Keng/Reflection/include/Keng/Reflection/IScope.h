#pragma once

#include "EverydayTools/GUID.h"

namespace keng::reflection
{
	class ScopeParameters
	{
	public:
		const char* name = nullptr;
		edt::GUID guid;
	};

	class IScope
	{
	public:
		virtual IScope& AddScope(const ScopeParameters& scopeParameters) = 0;
		virtual IScope* FindChild(const edt::GUID& guid) const = 0;
		virtual const edt::GUID& GetGUID() const = 0;
		virtual const char* GetName() const = 0;
		virtual ~IScope() = default;
	};
}