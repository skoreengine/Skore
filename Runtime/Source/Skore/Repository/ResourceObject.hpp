// Copyright 2023 Paulo Marangoni.
// Use of this source code is governed by the license that can be found in the LICENSE file at the root of this distribution.

#pragma once

#include "RepositoryTypes.hpp"

namespace Skore
{

	class SK_API ResourceObject
	{
	public:
		ResourceObject(ResourceStorage* resourceStorage, CPtr memory, bool ownMemory);
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
			SetPtr(name, &value);
		}

		template<typename T>
		void Set(u32 index, const T& value)
		{
			SetPtr(index, &value);
		}


		void Set(const StringView& name, ConstCPtr value)
		{
			SetPtr(name, value);
		}

		void Set(u32 index, ConstCPtr value)
		{
			SetPtr(index, &value);
		}

		explicit operator bool() const
		{
			return m_ResourceStorage != nullptr && m_Memory != nullptr;
		}

		void Commit();

	private:
		ResourceStorage* m_ResourceStorage;
		CPtr             m_Memory;
		bool             m_OwnMemory;

		ConstCPtr GetPtr(const StringView& name) const;
		ConstCPtr GetPtr(u32 index) const;
		void SetPtr(const StringView& name, ConstCPtr ptr);
		void SetPtr(u32 index, ConstCPtr ptr);
	};

}