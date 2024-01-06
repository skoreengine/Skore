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

		inline ConstCPtr GetValue(const StringView& name) const
		{
			return GetValuePtr(name);
		}

		inline  ConstCPtr GetValue(u32 index) const
		{
			return GetValuePtr(index);
		}

		template<typename T>
		const T& GetValue(const StringView& name) const
		{
			return *static_cast<const T*>(GetValuePtr(name));
		}

		template<typename T>
		const T& GetValue(u32 index) const
		{
			return *static_cast< const T*>(GetValuePtr(index));
		}

		template<typename T>
		void SetValue(const StringView& name, const T& value)
		{
			SetValuePtr(name, &value, GetTypeID<T>());
		}

		template<typename T>
		void SetValue(u32 index, const T& value)
		{
			SetValuePtr(index, &value, GetTypeID<T>());
		}

		void SetValue(const StringView& name, ConstCPtr value, TypeID typeId)
		{
			SetValuePtr(name, value, typeId);
		}

		void SetValue(u32 index, ConstCPtr value, TypeID typeId)
		{
			SetValuePtr(index, &value, typeId);
		}

		void SetSubobject(u32 index, RID subobject);

		void SetSubobject(const StringView& name, RID subobject);


		explicit operator bool() const
		{
			return m_ReadData != nullptr;
		}

		bool Has(u32 index) const
		{
			return GetValuePtr(index) != nullptr;
		}

		bool Has(const StringView& name) const
		{
			return GetValuePtr(name) != nullptr;
		}

		void Commit();

	private:
		ResourceData*    m_ReadData;
		ResourceData*    m_WriteData;

		ConstCPtr GetValuePtr(const StringView& name) const;
		ConstCPtr GetValuePtr(u32 index) const;
		void SetValuePtr(const StringView& name, ConstCPtr ptr, TypeID typeId);
		void SetValuePtr(u32 index, ConstCPtr ptr, TypeID typeId);
	};

}