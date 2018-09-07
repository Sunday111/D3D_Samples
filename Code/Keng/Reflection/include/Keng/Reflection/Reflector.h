#pragma once

#include "Keng/Reflection/TypeInfo.h"

namespace keng::reflection
{
	template<typename T>
	class Reflector
	{
	public:
		Reflector(TypeInfo<T>& typeInfo);

	private:
		TypeInfo<T>& m_typeInfo;
	};

	template<typename T>
	Reflector<T>::Reflector(TypeInfo<T>& typeInfo) :
		m_typeInfo(typeInfo)
	{}
}