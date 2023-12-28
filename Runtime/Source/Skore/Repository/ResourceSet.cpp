// Copyright 2023 Paulo Marangoni.
// Use of this source code is governed by the license that can be found in the LICENSE file at the root of this distribution.

#include "ResourceSet.hpp"

namespace Skore
{
	void ResourceSet::SetInt(u32 index, i64 value)
	{

	}

	void ResourceSet::SetFloat(u32 index, f64 value)
	{

	}

	void ResourceSet::SetBool(u32 index, bool value)
	{

	}

	void ResourceSet::SetString(u32 index, const StringView& value)
	{

	}

	void ResourceSet::SetInt(const StringView& name, i64 value)
	{

	}

	void ResourceSet::SetFloat(const StringView& name, f64 value)
	{

	}

	void ResourceSet::SetBool(const StringView& name, bool value)
	{

	}

	void ResourceSet::SetString(const StringView& name, const StringView& value)
	{

	}

	i64 ResourceSet::GetInt(u32 index) const
	{
		return 0;
	}

	f64 ResourceSet::GetFloat(u32 index) const
	{
		return 0;
	}

	bool ResourceSet::GetBool(u32 index) const
	{
		return false;
	}

	StringView ResourceSet::GetString(u32 index) const
	{
		return Skore::StringView();
	}

	i64 ResourceSet::GetInt(const StringView& name) const
	{
		return 0;
	}

	f64 ResourceSet::GetFloat(const StringView& name) const
	{
		return 0;
	}

	bool ResourceSet::GetBool(const StringView& name) const
	{
		return false;
	}

	StringView ResourceSet::GetString(const StringView& name) const
	{
		return Skore::StringView();
	}

	void ResourceSet::Commit()
	{

	}
}