// Copyright 2023 Paulo Marangoni.
// Use of this source code is governed by the license that can be found in the LICENSE file at the root of this distribution.

#pragma once

#include "Skore/Defines.hpp"
#include "StringView.hpp"
#include "String.hpp"
#include "TypeID.hpp"

namespace Skore
{
	struct TypeInfo
	{
		TypeID typeId{};
	};

	template<typename Type>
	TypeInfo MakeTypeInfo()
	{
		return TypeInfo{
			.typeId = GetTypeID<Type>()
		};
	}

	class SK_API ReflType
	{
	public:
		ReflType(const StringView& name, u32 version);
	private:
		String m_name{};
		u32    m_version{};
	};



	template<typename Type>
	class NativeTypeImpl
	{
	public:
		explicit NativeTypeImpl(ReflType& reflType) : m_reflType(reflType)
		{
		}

	private:
		ReflType& m_reflType;
	};


	namespace Reflection
	{

		SK_API void Shutdown();

		SK_API ReflType& NewType(const StringView& name, const TypeInfo& typeInfo);

		template<typename T>
		SK_API decltype(auto) Type()
		{
			return NativeTypeImpl<T>(NewType(GetTypeName<T>(), MakeTypeInfo<T>()));
		}

	}
}