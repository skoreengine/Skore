// Copyright 2023 Paulo Marangoni.
// Use of this source code is governed by the license that can be found in the LICENSE file at the root of this distribution.

#pragma once

#include "RepositoryTypes.hpp"
#include "Skore/Core/TypeInfo.hpp"

namespace Skore
{

	class SK_API ResourceObject
	{
	public:
		ResourceObject(ResourceData* readData,  ResourceData* writeData);
		ResourceObject(const ResourceObject& object) = delete;
		ResourceObject& operator=(const ResourceObject& object) = delete;

		virtual ~ResourceObject();

		inline ConstCPtr Get(const StringView& name) const
		{
			return GetPtr(name);
		}

		inline  ConstCPtr Get(u32 index) const
		{
			return GetPtr(index);
		}

		template<typename T>
		const T& Get(const StringView& name) const
		{
			return *static_cast<const T*>(GetPtr(name));
		}

		template<typename T>
		const T& Get(u32 index) const
		{
			return *static_cast< const T*>(GetPtr(index));
		}

		template<typename T>
		void Set(const StringView& name, const T& value)
		{
			SetPtr(name, &value, GetTypeID<T>());
		}

		template<typename T>
		void Set(u32 index, const T& value)
		{
			SetPtr(index, &value, GetTypeID<T>());
		}

		void Set(const StringView& name, ConstCPtr value, TypeID typeId)
		{
			SetPtr(name, value, typeId);
		}

		void Set(u32 index, ConstCPtr value, TypeID typeId)
		{
			SetPtr(index, &value, typeId);
		}

		explicit operator bool() const
		{
			return m_ReadData != nullptr;
		}

		bool Has(u32 index) const
		{
			return GetPtr(index) != nullptr;
		}

		bool Has(const StringView& name) const
		{
			return GetPtr(name) != nullptr;
		}

		void Commit();

	private:
		ResourceData*    m_ReadData;
		ResourceData*    m_WriteData;

		ConstCPtr GetPtr(const StringView& name) const;
		ConstCPtr GetPtr(u32 index) const;
		void SetPtr(const StringView& name, ConstCPtr ptr, TypeID typeId);
		void SetPtr(u32 index, ConstCPtr ptr, TypeID typeId);
	};

}