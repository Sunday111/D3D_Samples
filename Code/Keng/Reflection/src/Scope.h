#pragma once

#include <string_view>
#include <vector>
#include "Keng/Reflection/IScope.h"

namespace keng::reflection
{
	class Scope : public IScope
	{
	public:
		static constexpr bool PreventDuplicatedNames = true;

		Scope(const ScopeParameters& scopeParameters);

		virtual IScope& AddScope(const ScopeParameters& scopeParameters) override final;
		virtual IScope* FindChild(const edt::GUID& guid) const override final;
		virtual const edt::GUID& GetGUID() const override final;
		virtual const char* GetName() const override final;

	private:
		using Collection = std::vector<Scope>;
		using Iterator = typename Collection::iterator;
		using ConstIterator = typename Collection::const_iterator;

		ConstIterator LowerBound(const edt::GUID& guid) const;
		const Scope* FindChildByName(const std::string_view& name, bool recursive) const;

	private:
		edt::GUID m_guid;
		std::string m_name;
		std::vector<Scope> m_children;
	};
}
