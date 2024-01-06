// Copyright 2023 Paulo Marangoni.
// Use of this source code is governed by the license that can be found in the LICENSE file at the root of this distribution.

#include <atomic>
#include <mutex>
#include <queue>
#include "Repository.hpp"
#include "Skore/Core/HashMap.hpp"
#include "Skore/Core/SharedPtr.hpp"
#include "Skore/Core/Reflection.hpp"
#include "ResourceObject.hpp"

#define PAGE(value)    u32((value)/SK_REPO_PAGE_SIZE)
#define OFFSET(value)  (u32)((value) & (SK_REPO_PAGE_SIZE - 1))

#include "moodycamel/concurrentqueue.h"

namespace Skore::Repository
{
	ResourceStorage* GetOrAllocate(RID rid);
	RID NewId();
	void DestroyData(ResourceData* data, bool destroySubObjects);
	void DestroyResource(ResourceStorage* resourceStorage);
}

namespace Skore
{
	struct ResourceStorage;
	typedef ResourceStorage* ResourceStoragePtr;

	struct ResourceField
	{
		String            Name{};
		usize             Index{};
		ResourceFieldType FieldType{};
		TypeHandlerPtr    TypeHandler{};
		usize             Offset{};
	};

	typedef ResourceField* ResourceFieldPtr;

	struct ResourceType
	{
		TypeID                                    TypeId;
		usize                                     Size;
		HashMap<String, SharedPtr<ResourceField>> FieldsByName;
		Array<ResourceFieldPtr>                   FieldsByIndex;
	};

	typedef ResourceType* ResourceTypePtr;

	struct ResourceData
	{
		ResourceStoragePtr Storage{};
		CPtr               Memory{};
		Array<CPtr>        Fields{};
		ResourceStoragePtr Owner{};
	};

	typedef ResourceData* ResourceDataPtr;

	struct ResourceStorage
	{
		RID                          Rid;
		ResourceTypePtr              ResourceType{};
		std::atomic<ResourceDataPtr> Data;
		ResourceStoragePtr           Prototype{};
	};

	struct ResourcePage
	{
		ResourceStorage Elements[SK_REPO_PAGE_SIZE];
	};

	typedef ResourcePage* ResourcePagePtr;

	struct DestroyResourceData
	{
		ResourceDataPtr Data{};
		bool            DestroySubObjects{};
		bool            DestroyResource{};
	};

	struct RepositoryContext
	{
		AllocatorPtr Allocator;

		HashMap<TypeID, SharedPtr<ResourceType>> ResourceTypes{};

		std::atomic_size_t Counter{};
		usize              PageCount{};
		ResourcePagePtr    Pages[SK_REPO_PAGE_SIZE]{};
		std::mutex         PageMutex{};

		moodycamel::ConcurrentQueue<DestroyResourceData> ToCollectItems = moodycamel::ConcurrentQueue<DestroyResourceData>(100);
	};

	RepositoryContext repository{};

	void Repository::Init()
	{
		Init(GetDefaultAllocator());
	}

	void Repository::Init(Allocator* allocator)
	{
		repository.Allocator = allocator;
		CreateResource({});
	}

	void Repository::Shutdown()
	{
		GarbageCollect();

		for (u64 i = 0; i < repository.Counter; ++i)
		{
			ResourceStorage* storage = &repository.Pages[PAGE(i)]->Elements[OFFSET(i)];
			DestroyData(storage->Data, false);
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
		repository.Counter   = 0;
		repository.ResourceTypes.Clear();
	}

	void Repository::CreateResourceType(const ResourceTypeCreation& resourceTypeCreation)
	{
		SharedPtr<ResourceType> resourceType = MakeShared<ResourceType>();

		resourceType->TypeId = resourceTypeCreation.TypeId;
		resourceType->FieldsByIndex.Resize(resourceTypeCreation.Fields.Size());

		for (const ResourceFieldCreation& resourceFieldCreation: resourceTypeCreation.Fields)
		{
			SK_ASSERT(resourceFieldCreation.Index < resourceTypeCreation.Fields.Size(), "The index value cannot be greater than the number of elements");

			auto it = resourceType->FieldsByName.Emplace(resourceFieldCreation.Name, MakeShared<ResourceField>(resourceFieldCreation.Name,
				resourceFieldCreation.Index,
				resourceFieldCreation.Type)).First;

			SK_ASSERT(!resourceType->FieldsByIndex[resourceFieldCreation.Index], "Index duplicated");
			resourceType->FieldsByIndex[resourceFieldCreation.Index] = it->Second.Get();

			if (resourceFieldCreation.Type == ResourceFieldType_Value)
			{
				it->Second->TypeHandler = Reflection::FindTypeById(resourceFieldCreation.FieldTypeId);
				SK_ASSERT(it->Second->TypeHandler, "Type not found");
				it->Second->Offset = resourceType->Size;
				if (it->Second->TypeHandler)
				{
					resourceType->Size += it->Second->TypeHandler->GetTypeInfo().Size;
				}
			}
			else if (resourceFieldCreation.Type == ResourceFieldType_SubObject)
			{
				it->Second->Offset = resourceType->Size;
				resourceType->Size += sizeof(RID);
			}
			else if (resourceFieldCreation.Type == ResourceFieldType_SubObjectSet)
			{
				it->Second->Offset = resourceType->Size;
				resourceType->Size += sizeof(RID);
			}
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

	ResourceStorage* Repository::GetOrAllocate(RID rid)
	{
		if (repository.Pages[rid.Page] == nullptr)
		{
			std::unique_lock<std::mutex> lock(repository.PageMutex);
			if (repository.Pages[rid.Page] == nullptr)
			{
				repository.Pages[rid.Page] = static_cast<ResourcePage*>(repository.Allocator->MemAlloc(repository.Allocator->Alloc, sizeof(ResourcePage)));
				repository.PageCount++;
			}
		}
		return &repository.Pages[rid.Page]->Elements[rid.Offset];
	}

	void Repository::DestroyData(ResourceData* data, bool destroySubObjects)
	{
		if (data)
		{
			if (data->Memory)
			{
				for (int i = 0; i < data->Fields.Size(); ++i)
				{
					if (data->Fields[i] != nullptr)
					{
						if (destroySubObjects)
						{
							if (data->Storage->ResourceType->FieldsByIndex[i]->FieldType == ResourceFieldType_SubObject)
							{
								RID suboject = *static_cast<RID*>(data->Fields[i]);
								DestroyResource(&repository.Pages[suboject.Page]->Elements[suboject.Offset]);
							}
						}

						if (data->Storage->ResourceType->FieldsByIndex[i]->TypeHandler)
						{
							data->Storage->ResourceType->FieldsByIndex[i]->TypeHandler->Destructor(data->Fields[i]);
						}

						data->Fields[i] = nullptr;
					}

				}
				repository.Allocator->MemFree(repository.Allocator->Alloc, data->Memory, data->Storage->ResourceType->Size);
				data->Memory = nullptr;
			}
			Destroy(repository.Allocator, data);
		}
	}

	RID Repository::CreateResource(TypeID typeId)
	{
		RID rid = NewId();
		ResourceStorage* resourceStorage = GetOrAllocate(rid);
		ResourceType   * resourceType    = nullptr;

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
		ResourceStorage* resourceStorage  = GetOrAllocate(rid);
		ResourceStorage* prototypeStorage = &repository.Pages[prototype.Page]->Elements[prototype.Offset];
		SK_ASSERT(prototypeStorage->ResourceType, "Prototype can't be created from resources without types");
		SK_ASSERT(prototypeStorage->Data, "Prototype can't be created from null resources");

		ResourceData* data = Alloc<ResourceData>();
		data->Storage = resourceStorage;
		data->Memory  = nullptr;
		data->Fields.Resize(prototypeStorage->Data.load()->Fields.Size());

		new(PlaceHolder(), resourceStorage) ResourceStorage{
			.Rid = rid,
			.ResourceType = prototypeStorage->ResourceType,
			.Data = data,
			.Prototype = prototypeStorage
		};
		return rid;
	}

	ResourceObject Repository::Read(RID rid)
	{
		ResourceStorage* storage = &repository.Pages[rid.Page]->Elements[rid.Offset];
		return ResourceObject{storage->Data, nullptr};
	}

	ResourceObject Repository::Write(RID rid)
	{
		ResourceStorage* storage = &repository.Pages[rid.Page]->Elements[rid.Offset];
		CPtr memory = repository.Allocator->MemAlloc(repository.Allocator->Alloc, storage->ResourceType->Size);
		ResourceData* data = Alloc<ResourceData>(repository.Allocator, storage, memory, Array<CPtr>(storage->ResourceType->FieldsByIndex.Size()));

		if (storage->Data)
		{
			ResourceData* storageData = storage->Data.load();
			for (int i = 0; i < storage->ResourceType->FieldsByIndex.Size(); ++i)
			{
				if (storageData->Fields[i] != nullptr)
				{
					data->Fields[i] = static_cast<char*>(memory) + storage->ResourceType->FieldsByIndex[i]->Offset;
					storage->ResourceType->FieldsByIndex[i]->TypeHandler->Copy(storageData->Fields[i], data->Fields[i]);
				}
			}
		}
		return ResourceObject{storage->Data, data};
	}

	void Repository::GarbageCollect()
	{
		DestroyResourceData data;
		while (repository.ToCollectItems.try_dequeue(data))
		{
			if (data.DestroyResource)
			{
				DestroyResource(data.Data->Storage);
			}
			else
			{
				DestroyData(data.Data, data.DestroySubObjects);
			}
		}
	}

	void Repository::ClearValues(RID rid)
	{
		ResourceStorage* storage = &repository.Pages[rid.Page]->Elements[rid.Offset];
		if (storage->Data)
		{
			ResourceData* data = storage->Data.load();
			if (data->Memory)
			{
				for (int i = 0; i < data->Fields.Size(); ++i)
				{
					if (data->Fields[i] != nullptr)
					{
						data->Storage->ResourceType->FieldsByIndex[i]->TypeHandler->Destructor(data->Fields[i]);
						data->Fields[i] = nullptr;
					}
				}
				repository.Allocator->MemFree(repository.Allocator->Alloc, data->Memory, data->Storage->ResourceType->Size);
				data->Memory = nullptr;
			}
		}
	}

	void Repository::DestroyResource(RID rid)
	{
		ResourceStorage* storage = &repository.Pages[rid.Page]->Elements[rid.Offset];
		repository.ToCollectItems.enqueue(DestroyResourceData{
			.Data = storage->Data,
			.DestroySubObjects = true,
			.DestroyResource = true
		});
	}

	void Repository::DestroyResource(ResourceStoragePtr resourceStorage)
	{
		DestroyData(resourceStorage->Data, true);
		resourceStorage->~ResourceStorage();
		memset(resourceStorage, 0, sizeof(ResourceStorage));
	}

	RID Repository::CloneResource(RID rid)
	{
		return RID();
	}

	bool Repository::IsAlive(RID rid)
	{
		ResourceStorage* storage = &repository.Pages[rid.Page]->Elements[rid.Offset];
		return storage->Rid.Id != 0;
	}

	ResourceObject::ResourceObject(ResourceData* readData, ResourceData* writeData) : m_ReadData(readData), m_WriteData(writeData)
	{
	}

	ConstCPtr ResourceObject::GetValuePtr(const StringView& name) const
	{
		if (auto it = m_ReadData->Storage->ResourceType->FieldsByName.Find(name))
		{
			return GetValuePtr(it->Second->Index);
		}
		return nullptr;
	}

	ConstCPtr ResourceObjectGetValue(ResourceData* data, u32 index)
	{
		ConstCPtr ptr = data->Fields[index];
		if (!ptr && data->Storage->Prototype)
		{
			return ResourceObjectGetValue(data->Storage->Prototype->Data, index);
		}
		return ptr;
	}

	ConstCPtr ResourceObject::GetValuePtr(u32 index) const
	{
		return ResourceObjectGetValue(m_ReadData, index);
	}

	void ResourceObject::SetValuePtr(const StringView& name, ConstCPtr ptr, TypeID typeId)
	{
		if (auto it = m_WriteData->Storage->ResourceType->FieldsByName.Find(name))
		{
			SetValuePtr(it->Second->Index, ptr, typeId);
		}
	}

	void ResourceObject::SetValuePtr(u32 index, ConstCPtr ptr, TypeID typeId)
	{
		ResourceField* field = m_WriteData->Storage->ResourceType->FieldsByIndex[index];
		SK_ASSERT(field->FieldType == ResourceFieldType_Value, "Field is not ResourceFieldType_Value");
		SK_ASSERT(field->TypeHandler->GetTypeInfo().TypeId == typeId, "type is not the same");

		if (m_WriteData->Fields[index] == nullptr)
		{
			m_WriteData->Fields[index] = static_cast<char*>(m_WriteData->Memory) + field->Offset;
		}

		field->TypeHandler->Copy(ptr, m_WriteData->Fields[index]);
	}

	void ResourceObject::SetSubobject(u32 index, RID subobject)
	{
		ResourceField* field = m_WriteData->Storage->ResourceType->FieldsByIndex[index];
		SK_ASSERT(field->FieldType == ResourceFieldType_SubObject, "Field is not ResourceFieldType_SubObject");

		if (m_WriteData->Fields[index] == nullptr)
		{
			m_WriteData->Fields[index] = static_cast<char*>(m_WriteData->Memory) + field->Offset;
		}
		new(PlaceHolder(), m_WriteData->Fields[index]) RID{subobject};
	}

	void ResourceObject::SetSubobject(const StringView& name, RID subobject)
	{
		if (auto it = m_WriteData->Storage->ResourceType->FieldsByName.Find(name))
		{
			SetSubobject(it->Second->Index, subobject);
		}
	}

	RID ResourceObject::GetSubobject(u32 index)
	{
		return *static_cast<const RID*>(ResourceObjectGetValue(m_ReadData, index));
	}

	RID ResourceObject::GetSubobject(const StringView& name)
	{
		if (auto it = m_WriteData->Storage->ResourceType->FieldsByName.Find(name))
		{
			return GetSubobject(it->Second->Index);
		}
		return {};
	}

	void ResourceObject::Commit()
	{
		if (m_ReadData)
		{
			if (m_WriteData->Storage->Data.compare_exchange_strong(m_ReadData, m_WriteData))
			{
				repository.ToCollectItems.enqueue(DestroyResourceData{
					.Data = m_ReadData,
					.DestroySubObjects = true,
					.DestroyResource = false
				});

				m_WriteData = nullptr;
			}
		}
		else
		{
			m_WriteData->Storage->Data = m_WriteData;
			m_WriteData = nullptr;
		}
	}

	ResourceObject::~ResourceObject()
	{
		if (m_WriteData)
		{
			Repository::DestroyData(m_WriteData, true);
		}
	}

}