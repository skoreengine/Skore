// Copyright 2023 Paulo Marangoni.
// Use of this source code is governed by the license that can be found in the LICENSE file at the root of this distribution.

#include "Reflection.hpp"
#include "HashMap.hpp"
#include "String.hpp"
#include "SharedPtr.hpp"
#include "Array.hpp"
#include "Logger.hpp"

namespace Skore
{
	struct ReflectionContext
	{
		HashMap<String, Array<SharedPtr<TypeHandler>>> TypesByName{};
		HashMap<TypeID, Array<SharedPtr<TypeHandler>>> TypesByID{};

		Logger& Logger = Logger::GetLogger("Reflection");

		virtual ~ReflectionContext()
		{
			SK_ASSERT(TypesByName.Empty(), "Was the Reflection::Shutdown called?");
			SK_ASSERT(TypesByID.Empty(), "Was the Reflection::Shutdown called?");
		}
	};

	ReflectionContext reflection{};

	TypeHandler::TypeHandler(const StringView& name, u32 version) : m_Name(name), m_Version(version)
	{
	}

	void TypeHandler::SetFnDestroy(FnDestroy fnDestroy)
	{
		m_FnDestroy = fnDestroy;
	}

	void TypeHandler::SetFnCopy(FnCopy fnCopy)
	{
		m_FnCopy = fnCopy;
	}

	ConstructorHandler& TypeHandler::NewConstructor(TypeID* ids, usize size)
	{
		u64 constructorId = size > 0 ? MurmurHash64(ids, size * sizeof(TypeID), HashSeed64) : 0;
		auto it = m_Constructors.Find(constructorId);

		if (it == m_Constructors.end())
		{
			it = m_Constructors.Emplace(constructorId, MakeShared<ConstructorHandler>()).First;
			m_ConstructorArray.EmplaceBack(it->Second.Get());
		}
		return *it->Second;
	}

	ConstructorHandler* TypeHandler::FindConstructor(TypeID* ids, usize size) const
	{
		u64 constructorId = size > 0 ? MurmurHash64(ids, size * sizeof(TypeID), HashSeed64) : 0;
		if (auto it = m_Constructors.Find(constructorId))
		{
			return it->Second.Get();
		}
		return nullptr;
	}

	Span<ConstructorHandler*> TypeHandler::GetConstructors() const
	{
		return m_ConstructorArray;
	}

	FieldHandler& TypeHandler::NewField(const StringView& fieldName)
	{
		auto it = m_Fields.Find(fieldName);
		if (it == m_Fields.end())
		{
			it = m_Fields.Emplace(fieldName, MakeShared<FieldHandler>(fieldName)).First;
			m_FieldArray.EmplaceBack(it->Second.Get());
		}
		return *it->Second;
	}

	FieldHandler* TypeHandler::FindField(const StringView& fieldName) const
	{
		if (auto it = m_Fields.Find(fieldName))
		{
			return it->Second.Get();
		}
		return nullptr;
	}

	Span<FieldHandler*> TypeHandler::GetFields() const
	{
		return m_FieldArray;
	}

	FunctionHandler& TypeHandler::NewFunction(const FunctionHandlerCreation& creation)
	{
		auto it = m_Functions.Find(creation.Name);
		if (it == m_Functions.end())
		{
			it = m_Functions.Emplace(String{creation.Name}, MakeShared<FunctionHandler>(creation)).First;
			m_FunctionArray.EmplaceBack(it->Second.Get());
		}
		return *it->Second;
	}

	FunctionHandler* TypeHandler::FindFunction(const StringView& functionName) const
	{
		if (auto it = m_Functions.Find(functionName))
		{
			return it->Second.Get();
		}
		return nullptr;
	}

	Span<FunctionHandler*> TypeHandler::GetFunctions() const
	{
		return m_FunctionArray;
	}

	AttributeHandler& TypeHandler::NewAttribute(TypeID attributeId)
	{
		auto it = m_Attributes.Find(attributeId);
		if (it == m_Attributes.end())
		{
			it = m_Attributes.Emplace(attributeId, MakeShared<AttributeHandler>()).First;
			m_AttributeArray.EmplaceBack(it->Second.Get());
		}
		return *it->Second;
	}

	void TypeHandler::Destroy(CPtr instance, Allocator* allocator) const
	{
		if (m_FnDestroy)
		{
			m_FnDestroy(this, allocator, instance);
		}
	}

	void TypeHandler::Copy(ConstCPtr source, CPtr dest) const
	{
		if (m_FnCopy)
		{
			m_FnCopy(this, source, dest);
		}
	}

	ConstCPtr TypeHandler::GetAttribute(TypeID attributeId) const
	{
		if (auto it = m_Attributes.Find(attributeId))
		{
			return it->Second->GetValue();
		}
		return nullptr;
	}

	TypeHandler& Reflection::NewType(const StringView& name, const TypeInfo& typeInfo)
	{
		auto itByName = reflection.TypesByName.Find(name);
		if (!itByName)
		{
			itByName = reflection.TypesByName.Emplace(name, Array<SharedPtr<TypeHandler>>{}).First;
		}

		auto itById = reflection.TypesByID.Find(typeInfo.TypeId);
		if (!itById)
		{
			itById = reflection.TypesByID.Emplace(typeInfo.TypeId, Array<SharedPtr<TypeHandler>>{}).First;
		}

		usize version = itByName->Second.Size() + 1;
		SharedPtr<TypeHandler> typeHandler = MakeShared<TypeHandler>(name, version);

		itByName->Second.EmplaceBack(typeHandler);
		itById->Second.EmplaceBack(typeHandler);

		reflection.Logger.Debug("Type {} Registered ", name, version);

		return *typeHandler;
	}

	void Reflection::Shutdown()
	{
		reflection.TypesByID.Clear();
		reflection.TypesByName.Clear();
	}

	TypeHandler* Reflection::FindTypeByName(const StringView& name)
	{
		if (auto it = reflection.TypesByName.Find(name))
		{
			return it->Second.Back().Get();
		}
		return nullptr;
	}

	TypeHandler* Reflection::FindTypeById(TypeID typeId)
	{
		if (auto it = reflection.TypesByID.Find(typeId))
		{
			return it->Second.Back().Get();
		}
		return nullptr;
	}

	void AttributeHandler::SetFnGetValue(AttributeHandler::FnGetValue fnGetValue)
	{
		m_FnGetValue = fnGetValue;
	}

	ConstCPtr AttributeHandler::GetValue()
	{
		if (m_FnGetValue)
		{
			return m_FnGetValue(this);
		}
		return nullptr;
	}

	void ConstructorHandler::SetPlacementNewFn(ConstructorHandler::PlacementNewFn placementNewFn)
	{
		m_PlacementNewFn = placementNewFn;
	}

	void ConstructorHandler::SetNewInstanceFn(ConstructorHandler::NewInstanceFn newInstanceFn)
	{
		m_NewInstanceFn = newInstanceFn;
	}

	CPtr ConstructorHandler::NewInstance(Allocator* allocator, CPtr* params)
	{
		if (m_NewInstanceFn)
		{
			return m_NewInstanceFn(this, allocator, params);
		}
		return nullptr;
	}

	FieldHandler::FieldHandler(const String& name) : m_Name(name)
	{}

	void FieldHandler::SetFnGetFieldInfo(FnGetFieldInfo fieldInfoFn)
	{
		m_FnGetFieldInfo = fieldInfoFn;
	}

	FieldInfo FieldHandler::GetFieldInfo() const
	{
		if (m_FnGetFieldInfo)
		{
			return m_FnGetFieldInfo(this);
		}
		return {};
	}

	CPtr FieldHandler::GetFieldPointer(CPtr instance) const
	{
		if (m_FnGetFieldPointer)
		{
			return m_FnGetFieldPointer(this, instance);
		}
		return nullptr;
	}

	void FieldHandler::SetFnGetFieldPointer(FieldHandler::FnGetFieldPointer fnGetFieldPointer)
	{
		m_FnGetFieldPointer = fnGetFieldPointer;
	}

	void FieldHandler::CopyValueTo(ConstCPtr instance, CPtr value)
	{
		if (m_FnCopyValueTo)
		{
			m_FnCopyValueTo(this, instance, value);
		}
	}

	void FieldHandler::SetValue(CPtr instance, ConstCPtr value)
	{
		if (m_FnSetValue)
		{
			m_FnSetValue(this, instance, value);
		}
	}

	void FieldHandler::SetFnCopyValueTo(FnCopyValueTo fnCopyValueTo)
	{
		m_FnCopyValueTo = fnCopyValueTo;
	}

	void FieldHandler::SetFnSetValue(FnSetValue fnSetValue)
	{
		m_FnSetValue = fnSetValue;
	}

	FunctionHandler::FunctionHandler(const FunctionHandlerCreation& creation) : m_Name(creation.Name), m_FunctionId(creation.FunctionId), m_Return(creation.Return)
	{
		//TODO SimpleName
		for (int i = 0; i < creation.Params.Size(); ++i)
		{
			m_Params.EmplaceBack(i, creation.Params[i]);
		}
	}

	StringView FunctionHandler::GetName() const
	{
		return m_Name;
	}

	Span<ParamHandler> FunctionHandler::GetParams() const
	{
		return m_Params;
	}

	FieldInfo FunctionHandler::GetReturn() const
	{
		return m_Return;
	}

	void FunctionHandler::SetFnCall(FnCall fnCall)
	{
		m_FnCall = fnCall;
	}

	void FunctionHandler::SetFunctionPointer(CPtr functionPointer)
	{
		m_FunctionPointer = functionPointer;
	}

	CPtr FunctionHandler::GetFunctionPointer() const
	{
		return m_FunctionPointer;
	}

	void FunctionHandler::Call(CPtr instance, CPtr ret, CPtr* params) const
	{
		if (m_FnCall)
		{
			m_FnCall(this, instance, ret, params);
		}
	}

	AttributeHandler& FunctionHandler::NewAttribute(TypeID attributeId)
	{
		auto it = m_Attributes.Find(attributeId);
		if (it == m_Attributes.end())
		{
			it = m_Attributes.Emplace(attributeId, MakeShared<AttributeHandler>()).First;
			m_AttributeArray.EmplaceBack(it->Second.Get());
		}
		return *it->Second;
	}

	ConstCPtr FunctionHandler::GetAttribute(TypeID attributeId) const
	{
		if (auto it = m_Attributes.Find(attributeId))
		{
			return it->Second->GetValue();
		}
		return nullptr;
	}

	ParamHandler::ParamHandler(usize index, const FieldInfo& fieldInfo) : m_FieldInfo(fieldInfo)
	{
		m_Name.Append("param_");
		m_Name.Append(index);
	}
}
