// Copyright 2023 Paulo Marangoni.
// Use of this source code is governed by the license that can be found in the LICENSE file at the root of this distribution.

#include <atomic>
#include <mutex>
#include "Repository.hpp"
#include "Skore/Core/HashMap.hpp"
#include "Skore/Core/SharedPtr.hpp"
#include "Skore/Core/Reflection.hpp"
#include "ResourceObject.hpp"

#define PAGE(value)    u32((value)/SK_REPO_PAGE_SIZE)
#define OFFSET(value)  (u32)((value) & (SK_REPO_PAGE_SIZE - 1))

namespace Skore::Repository
{
	ResourceStorage* GetOrAllocate(RID rid);
	RID NewId();
	void DestroyInternal(ResourceStorage* resourceStorage, CPtr memory);
}

namespace Skore
{

	struct ResourceField
	{
		String Name{};
		TypeHandler* Type{};
		usize  Offset{};
	};

	struct ResourceType
	{
		TypeID                                    TypeId;
		usize                                     Size;
		HashMap<String, SharedPtr<ResourceField>> FieldsByName;
		Array<ResourceField*>                     FieldsByIndex;
	};

	struct ResourceStorage
	{
		RID Rid;
		ResourceType* ResourceType;
		CPtr Memory;
	};

	struct ResourcePage
	{
		ResourceStorage Elements[SK_REPO_PAGE_SIZE];
	};

	struct RepositoryContext
	{
		Allocator* Allocator;
		HashMap<TypeID, SharedPtr<ResourceType>> ResourceTypes{};

		std::atomic_size_t Counter{};
		std::atomic_size_t PageCount{};
		ResourcePage* Pages[SK_REPO_PAGE_SIZE]{};
		std::mutex PageMutex{};
	};

	RepositoryContext repository{};

	void Repository::Init()
	{
		repository.Allocator = GetDefaultAllocator();
	}

	void Repository::Shutdown()
	{
		for (u64 i = 0; i < repository.Counter; ++i)
		{
			ResourceStorage* storage = &repository.Pages[PAGE(i)]->Elements[OFFSET(i)];
			DestroyInternal(storage, storage->Memory);
			storage->~ResourceStorage();
		}

		for (u64 i = 0; i < repository.PageCount; ++i)
		{
			if (!repository.Pages[i])
			{
				break;
			}
			repository.Allocator->MemFree(repository.Allocator->Alloc, repository.Pages[i], sizeof(ResourcePage));
			repository.Pages[i] = nullptr;
		}
		repository.Counter = 0;
		repository.ResourceTypes.Clear();
	}

	void Repository::CreateResourceType(const ResourceTypeCreation& resourceTypeCreation)
	{
		SharedPtr<ResourceType> resourceType = MakeShared<ResourceType>();

		resourceType->TypeId = resourceTypeCreation.TypeId;

		for (const ResourceFieldCreation& resourceFieldCreation: resourceTypeCreation.Fields)
		{
			TypeHandler* type = Reflection::FindTypeById(resourceFieldCreation.Type);
			SK_ASSERT(type, "Type not found");
			auto it = resourceType->FieldsByName.Emplace(resourceFieldCreation.Name, MakeShared<ResourceField>(resourceFieldCreation.Name, type)).First;
			resourceType->FieldsByIndex.EmplaceBack(it->Second.Get());
			//TODO improve alignment
			it->Second->Offset = resourceType->Size;
			resourceType->Size += type->GetTypeInfo().Size;
		}

		repository.ResourceTypes.Emplace(resourceTypeCreation.TypeId, Traits::Move(resourceType));
	}

	void Repository::CreateResourceType(TypeID typeId)
	{
		//TODO create from reflection TYPE.
	}

	SK_FINLINE RID Repository::NewId()
	{
		u64 index = repository.Counter++;
		return RID{OFFSET(index), PAGE(index)};
	}

	//internals
	ResourceStorage* Repository::GetOrAllocate(RID rid)
	{
		if (repository.PageCount <= rid.Page)
		{
			std::unique_lock<std::mutex> lock(repository.PageMutex);
			if (repository.Pages[rid.Page] == nullptr)
			{
				repository.Pages[rid.Page] = static_cast<ResourcePage*>(repository.Allocator->MemAlloc(repository.Allocator->Alloc, sizeof(ResourcePage)));
			}
			repository.PageCount++;
		}
		return &repository.Pages[rid.Page]->Elements[rid.Offset];
	}

	void Repository::DestroyInternal(ResourceStorage* resourceStorage, CPtr memory)
	{
		if (memory)
		{
			for (ResourceField* field: resourceStorage->ResourceType->FieldsByIndex)
			{
				field->Type->Destructor(static_cast<char*>(memory) + field->Offset);
			}
			repository.Allocator->MemFree(repository.Allocator->Alloc, memory, sizeof(resourceStorage->ResourceType->Size));
		}
	}

	RID Repository::CreateResource(TypeID typeId)
	{
		RID rid = NewId();
		ResourceStorage* resourceStorage = GetOrAllocate(rid);
		ResourceType* resourceType = nullptr;

		if (auto it = repository.ResourceTypes.Find(typeId))
		{
			resourceType = it->Second.Get();
		}

		new(PlaceHolder(), resourceStorage) ResourceStorage{
			.Rid = rid,
			.ResourceType = resourceType,
			.Memory = nullptr
		};

		return rid;
	}

	ResourceObject Repository::Read(RID rid)
	{
		ResourceStorage* storage = &repository.Pages[rid.Page]->Elements[rid.Offset];
		return ResourceObject{storage, storage->Memory, false};
	}

	ResourceObject Repository::Write(RID rid)
	{
		ResourceStorage* storage = &repository.Pages[rid.Page]->Elements[rid.Offset];
		CPtr memory = repository.Allocator->MemAlloc(repository.Allocator->Alloc, storage->ResourceType->Size);
		for (ResourceField* field: storage->ResourceType->FieldsByIndex)
		{
			field->Type->Construct(static_cast<char*>(memory) + field->Offset);
		}
		return ResourceObject{storage, memory, true};
	}

	ResourceObject::ResourceObject(ResourceStorage* resourceStorage, CPtr memory, bool ownMemory) : m_ResourceStorage(resourceStorage), m_Memory(memory), m_OwnMemory(ownMemory)
	{
	}

	ConstCPtr ResourceObject::GetPtr(const StringView& name) const
	{
		if (auto it = m_ResourceStorage->ResourceType->FieldsByName.Find(name))
		{
			return static_cast<char*>(m_Memory) + it->Second->Offset;
		}
		return nullptr;
	}

	ConstCPtr ResourceObject::GetPtr(u32 index) const
	{
		return static_cast<char*>(m_Memory) + m_ResourceStorage->ResourceType->FieldsByIndex[index]->Offset;
	}

	void ResourceObject::SetPtr(const StringView& name, ConstCPtr ptr)
	{
		if (auto it = m_ResourceStorage->ResourceType->FieldsByName.Find(name))
		{
			it->Second->Type->Copy(ptr, static_cast<char*>(m_Memory) + it->Second->Offset);
		}
	}

	void ResourceObject::SetPtr(u32 index, ConstCPtr ptr)
	{
		m_ResourceStorage->ResourceType->FieldsByIndex[index]->Type->Copy(ptr,
			static_cast<char*>(m_Memory) + m_ResourceStorage->ResourceType->FieldsByIndex[index]->Offset);
	}

	void ResourceObject::Commit()
	{
		Repository::DestroyInternal(m_ResourceStorage, m_ResourceStorage->Memory);
		m_ResourceStorage->Memory = m_Memory;
		m_Memory = nullptr;
	}

	ResourceObject::~ResourceObject()
	{
		if (m_OwnMemory && m_Memory)
		{
			Repository::DestroyInternal(m_ResourceStorage, m_Memory);
		}
	}

}