// Copyright 2023 Paulo Marangoni.
// Use of this source code is governed by the license that can be found in the LICENSE file at the root of this distribution.

#include "ResourceSet.hpp"

namespace Skore
{

	i32 dummy = 102;

	void ResourceSet::SetValue(const StringView& name, ConstCPtr value)
	{

	}

	void ResourceSet::SetValue(u32 index, ConstCPtr value)
	{

	}

	ConstCPtr ResourceSet::GetValue(const StringView& name) const
	{
		return &dummy;
	}

	ConstCPtr ResourceSet::GetValue(u32 index) const
	{
		return &dummy;
	}

	void ResourceSet::Commit()
	{

	}
}