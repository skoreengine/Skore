// Copyright 2023 Paulo Marangoni.
// Use of this source code is governed by the license that can be found in the LICENSE file at the root of this distribution.

#pragma once

#include "Skore/Defines.hpp"
#include "Skore/Core/StringView.hpp"


namespace Skore
{
	class SK_API ResourceSet
	{
	public:
		void SetValue(const StringView& name, ConstCPtr value);
		void SetValue(u32 index, ConstCPtr value);

		ConstCPtr GetValue(const StringView& name) const;
		ConstCPtr GetValue(u32 index) const;

		template<typename Type>
		void Set(const StringView& name, const Type& value)
		{
			SetValue(name, &value);
		}

		template<typename Type>
		void Set(u32 index, const Type& value)
		{
			SetValue(index, &value);
		}

		template<typename Type>
		const Type& Get(const StringView& name) const
		{
			return *static_cast<const Type*>(GetValue(name));
		}

		template<typename Type>
		const Type& Get(u32 index) const
		{
			return *static_cast<const Type*>(GetValue(index));
		}

		void Commit();

	private:

	};
}