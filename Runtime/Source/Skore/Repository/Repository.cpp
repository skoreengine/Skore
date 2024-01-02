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
	void DestroyData(ResourceStorage* resourceStorage, ResourceData* data);
}

namespace Skore
{

	struct ResourceField
	{
		String Name{};
		TypeHandler* Type{};
		usize Index{};
		usize Offset{};
	};

	struct ResourceType
	{
		TypeID                                    TypeId;
		usize                                     Size;
		HashMap<String, SharedPtr<ResourceField>> FieldsByName;
		Array<ResourceField*>                     FieldsByIndex;
	};

	struct ResourceData
	{
		CPtr Memory{};
		Array<CPtr> Fields{};
	};

	struct ResourceStorage
	{
		RID Rid;
		ResourceType* ResourceType{};
		ResourceData* Data{};
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
		CreateResource({});
	}

	void Repository::Shutdown()
	{
		for (u64 i = 0; i < repository.Counter; ++i)
		{
			ResourceStorage* storage = &repository.Pages[PAGE(i)]->Elements[OFFSET(i)];
			DestroyData(storage, storage->Data);
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
		repository.PageCount = 0;
		repository.Counter = 0;
		repository.ResourceTypes.Clear();
	}

	void Repository::CreateResourceType(const ResourceTypeCreation& resourceTypeCreation)
	{
		SharedPtr<ResourceType> resourceType = MakeShared<ResourceType>();

		resourceType->TypeId = resourceTypeCreation.TypeId;

		for (u32 index = 0; index < resourceTypeCreation.Fields.Size(); ++index)
		{
			const ResourceFieldCreation& resourceFieldCreation =  resourceTypeCreation.Fields[index];
			TypeHandler* type = Reflection::FindTypeById(resourceFieldCreation.Type);
			SK_ASSERT(type, "Type not found");
			auto it = resourceType->FieldsByName.Emplace(resourceFieldCreation.Name, MakeShared<ResourceField>(resourceFieldCreation.Name, type, index)).First;
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

	void Repository::DestroyData(ResourceStorage* resourceStorage, ResourceData* data)
	{
		if (data)
		{
			if (data->Memory)
			{
				u64 memoryLocation = (u64) data->Memory;
				for (int i = 0; i < data->Fields.Size(); ++i)
				{
					u64 ptr = ((u64) data->Fields[i]);
					if (ptr >= memoryLocation && ptr <= memoryLocation + resourceStorage->ResourceType->Size)
					{
						resourceStorage->ResourceType->FieldsByIndex[i]->Type->Destructor(data->Fields[i]);
					}
				}
				repository.Allocator->MemFree(repository.Allocator->Alloc, data->Memory, sizeof(resourceStorage->ResourceType->Size));
			}
			Destroy(repository.Allocator, data);
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
			.Data = {}
		};

		return rid;
	}

	RID Repository::CreateFromPrototype(RID prototype)
	{
		RID rid = NewId();
		ResourceStorage* resourceStorage = GetOrAllocate(rid);
		ResourceStorage* prototypeStorage = &repository.Pages[prototype.Page]->Elements[prototype.Offset];
		SK_ASSERT(prototypeStorage->ResourceType, "Prototype can't be created from resources without types");
		SK_ASSERT(prototypeStorage->Data, "Prototype can't be created from null resources");

		ResourceData* data = Alloc<ResourceData>();
		data->Memory = nullptr;
		data->Fields = prototypeStorage->Data->Fields;

		new(PlaceHolder(), resourceStorage) ResourceStorage{
			.Rid = rid,
			.ResourceType = prototypeStorage->ResourceType,
			.Data = data
		};
		return rid;
	}

	ResourceObject Repository::Read(RID rid)
	{
		ResourceStorage* storage = &repository.Pages[rid.Page]->Elements[rid.Offset];
		return ResourceObject{storage, storage->Data, false};
	}

	ResourceObject Repository::Write(RID rid)
	{
		ResourceStorage* storage = &repository.Pages[rid.Page]->Elements[rid.Offset];
		CPtr memory = repository.Allocator->MemAlloc(repository.Allocator->Alloc, storage->ResourceType->Size);
		return ResourceObject{storage, Alloc<ResourceData>(repository.Allocator, memory, Array<CPtr>(storage->ResourceType->FieldsByIndex.Size())), true};
	}

	ResourceObject::ResourceObject(ResourceStorage* resourceStorage, ResourceData* data, bool ownMemory) : m_ResourceStorage(resourceStorage), m_Data(data), m_OwnMemory(ownMemory)
	{
	}

	ConstCPtr ResourceObject::GetPtr(const StringView& name) const
	{
		if (auto it = m_ResourceStorage->ResourceType->FieldsByName.Find(name))
		{
			return GetPtr(it->Second->Index);
		}
		return nullptr;
	}

	ConstCPtr ResourceObject::GetPtr(u32 index) const
	{
		return m_Data->Fields[index];
	}

	void ResourceObject::SetPtr(const StringView& name, ConstCPtr ptr, TypeID typeId)
	{
		if (auto it = m_ResourceStorage->ResourceType->FieldsByName.Find(name))
		{
			SetPtr(it->Second->Index, ptr, typeId);
		}
	}

	void ResourceObject::SetPtr(u32 index, ConstCPtr ptr, TypeID typeId)
	{
		ResourceField* field = m_ResourceStorage->ResourceType->FieldsByIndex[index];
		SK_ASSERT(field->Type->GetTypeInfo().TypeId == typeId, "type is not the same");

		if (m_Data->Fields[index] == nullptr)
		{
			m_Data->Fields[index] = static_cast<char*>(m_Data->Memory) + field->Offset;
		}

		field->Type->Copy(ptr, m_Data->Fields[index]);
	}

	void ResourceObject::Commit()
	{
		ResourceData* original = m_ResourceStorage->Data;
		m_ResourceStorage->Data = m_Data;

		if (original)
		{
			u64 memoryLocation = (u64) original->Memory;

			for (int i = 0; i < m_ResourceStorage->Data->Fields.Size(); ++i)
			{
				if (m_ResourceStorage->Data->Fields[i] == nullptr && original->Fields[i] != nullptr)
				{
					u64 ptr = (u64) original->Fields[i];
					if (ptr >= memoryLocation && ptr <= (memoryLocation + m_ResourceStorage->ResourceType->Size))
					{
						m_ResourceStorage->ResourceType->FieldsByIndex[i]->Type->Copy(original->Fields[i], m_ResourceStorage->Data->Fields[i]);
					}
					else
					{
						m_ResourceStorage->Data->Fields[i] = original->Fields[i];
					}
				}
			}
			//TODO - defer
			Repository::DestroyData(m_ResourceStorage, original);
		}
		else
		{
			for (int i = 0; i < m_ResourceStorage->Data->Fields.Size(); ++i)
			{
				if (m_ResourceStorage->Data->Fields[i] == nullptr)
				{
					m_ResourceStorage->Data->Fields[i] = static_cast<char*>(m_ResourceStorage->Data->Memory) + m_ResourceStorage->ResourceType->FieldsByIndex[i]->Offset;
					m_ResourceStorage->ResourceType->FieldsByIndex[i]->Type->Construct(m_ResourceStorage->Data->Fields[i]);
				}
			}
		}

		m_OwnMemory = false;
		m_Data = nullptr;
	}

	ResourceObject::~ResourceObject()
	{
		//this should be called only in cases of writes with missing commits.
		if (m_OwnMemory && m_Data)
		{
			Repository::DestroyData(m_ResourceStorage, m_Data);
		}
	}

}