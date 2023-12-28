// Copyright 2023 Paulo Marangoni.
// Use of this source code is governed by the license that can be found in the LICENSE file at the root of this distribution.

#include "Repository.hpp"

namespace Skore
{

	void Repository::Init()
	{

	}

	void Repository::Shutdown()
	{

	}

	void Repository::CreateResourceType(const ResourceTypeCreation& resourceTypeCreation)
	{

	}

	void Repository::CreateResourceType(TypeID typeId)
	{

	}

	RID Repository::CreateResource(TypeID typeId)
	{
		return RID();
	}

	const ResourceSet* Repository::Read(RID rid)
	{
		return nullptr;
	}

	ResourceSet* Repository::Write(RID rid)
	{
		return nullptr;
	}



}