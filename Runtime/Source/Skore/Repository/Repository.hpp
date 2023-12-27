// Copyright 2023 Paulo Marangoni.
// Use of this source code is governed by the license that can be found in the LICENSE file at the root of this distribution.

#pragma once

#include "Skore/Defines.hpp"
#include "RepositoryTypes.hpp"

namespace Skore::Repository
{
	SK_API void             Init();

	SK_API RID              CreateObject(TypeID typeId);
	SK_API ConstCPtr        Read(RID rid);
	SK_API CPtr             Write(RID rid);
	SK_API void             Commit(RID rid, CPtr instance);

	SK_API void             Shutdown();

	template<typename T>
	const T* Read(RID rid)
	{
		return static_cast<const T*>(Read(rid));
	}

	template<typename T>
	T* Write(RID rid)
	{
		return static_cast<T*>(Write(rid));
	}
}