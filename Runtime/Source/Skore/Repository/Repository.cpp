// Copyright 2023 Paulo Marangoni.
// Use of this source code is governed by the license that can be found in the LICENSE file at the root of this distribution.

#include "Repository.hpp"

namespace Skore
{

	void Repository::Init()
	{

	}

	RID Repository::CreateObject(TypeID typeId)
	{
		return RID();
	}

	ConstCPtr Repository::Read(RID rid)
	{
		return nullptr;
	}

	CPtr Repository::Write(RID rid)
	{
		return nullptr;
	}


	void Repository::Shutdown()
	{

	}

	void Repository::Commit(RID rid, CPtr instance)
	{

	}

}