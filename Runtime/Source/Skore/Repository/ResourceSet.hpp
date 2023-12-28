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
		void SetInt(u32 index, i64 value);
		void SetFloat(u32 index, f64 value);
		void SetBool(u32 index, bool value);
		void SetString(u32 index, const StringView& value);

		void SetInt(const StringView& name, i64 value);
		void SetFloat(const StringView& name, f64 value);
		void SetBool(const StringView& name, bool value);
		void SetString(const StringView& name, const StringView& value);

		i64 GetInt(u32 index) const;
		f64 GetFloat(u32 index) const;
		bool GetBool(u32 index) const;
		StringView GetString(u32 index) const;

		i64 GetInt(const StringView& name) const;
		f64 GetFloat(const StringView& name) const;
		bool GetBool(const StringView& name) const;
		StringView GetString(const StringView& name) const;

		void Commit();
	private:

	};
}