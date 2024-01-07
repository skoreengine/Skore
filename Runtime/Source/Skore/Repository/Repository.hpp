// Copyright 2023 Paulo Marangoni.
// Use of this source code is governed by the license that can be found in the LICENSE file at the root of this distribution.

#pragma once

#include "Skore/Defines.hpp"
#include "Skore/Core/UUID.hpp"
#include "RepositoryTypes.hpp"
#include "ResourceObject.hpp"



namespace Skore::Repository
{
	SK_API void Init();
	SK_API void Init(Allocator* allocator);
	SK_API void Shutdown();
	SK_API void GarbageCollect();

	SK_API void CreateResourceType(const ResourceTypeCreation& resourceTypeCreation);
	SK_API void CreateResourceType(TypeID typeId);

	SK_API RID              CreateResource(TypeID typeId);
	SK_API RID              CreateFromPrototype(RID prototype);
	SK_API void             SetUUID(const RID& rid, const UUID& uuid);
	SK_API UUID             GetUUID(const RID& rid);
	SK_API RID              GetByUUID(const UUID& uuid);
	SK_API void             ClearValues(RID rid);
	SK_API void             DestroyResource(RID rid);
	SK_API RID              CloneResource(RID rid);
	SK_API ResourceObject   Read(RID rid);
	SK_API ResourceObject   Write(RID rid);
	SK_API bool             IsAlive(RID rid);

}