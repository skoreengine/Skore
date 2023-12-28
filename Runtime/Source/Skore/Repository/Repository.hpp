// Copyright 2023 Paulo Marangoni.
// Use of this source code is governed by the license that can be found in the LICENSE file at the root of this distribution.

#pragma once

#include "Skore/Defines.hpp"
#include "RepositoryTypes.hpp"
#include "ResourceSet.hpp"

namespace Skore::Repository
{
	SK_API void Init();
	SK_API void Shutdown();

	SK_API void CreateResourceType(const ResourceTypeCreation& resourceTypeCreation);
	SK_API void CreateResourceType(TypeID typeId);

	SK_API RID                          CreateResource(TypeID typeId);
	SK_API const ResourceSet*           Read(RID rid);
	SK_API ResourceSet*                 Write(RID rid);

}