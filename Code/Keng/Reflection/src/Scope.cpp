#include <algorithm>
#include "Scope.h"
#include "EverydayTools/Exception/CallAndRethrow.h"
#include "EverydayTools/Exception/ThrowIfFailed.h"

namespace keng::reflection
{
	IScope& Scope::AddScope(const ScopeParameters& scopeParameters) {
		return CallAndRethrowM + [&] () -> IScope& {
			if constexpr (PreventDuplicatedNames) {
				const Scope* duplicate = FindChildByName(scopeParameters.name, false);
				edt::ThrowIfFailed(duplicate == nullptr, "Name duplicate!");
			}
			auto it = LowerBound(scopeParameters.guid);
			edt::ThrowIfFailed(it == m_children.end() || (it->GetGUID() != scopeParameters.guid), "GUID duplicate!");
			return m_children.emplace_back(scopeParameters);
		};
	}

	const IScope* Scope::FindChild(const edt::GUID& guid) const {
		return CallAndRethrowM + [&] () -> const IScope* {
			auto it = LowerBound(guid);
			if (it == m_children.end() || (it->GetGUID() != guid)) {
				return &(*it);
			}

			return nullptr;
		};
	}

	IScope* Scope::FindChild(const edt::GUID& guid) {
		// Reuse const version
		auto const_this = const_cast<const Scope*>(this);
		const IScope* const_child = const_this->FindChild(guid);
		return const_cast<IScope*>(const_child);
	}

	const edt::GUID& Scope::GetGUID() const {
		return m_guid;
	}

	const char* Scope::GetName() const {
		return m_name.c_str();
	}

	Scope::ConstIterator Scope::LowerBound(const edt::GUID& guid) const {
		return CallAndRethrowM + [&] {
			return std::lower_bound(m_children.begin(), m_children.end(), guid, [](const Scope& scope, const edt::GUID& guid) {
				return scope.GetGUID() < guid;
			});
		};
	}

	const Scope* Scope::FindChildByName(const std::string_view& name, bool recursive) const {
		return CallAndRethrowM + [&] () -> const Scope* {
			for (const Scope& child: m_children) {
				if (child.GetName() == name) {
					return &child;
				}

				if (recursive) {
					auto found = child.FindChildByName(name, recursive);
					if (found) {
						return found;
					}
				}
			}

			return nullptr;
		};
	}

	Scope::Scope(const ScopeParameters& scopeParameters) :
		m_name(scopeParameters.name),
		m_guid(scopeParameters.guid)
	{

	}

}