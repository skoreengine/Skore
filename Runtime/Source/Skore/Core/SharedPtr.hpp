// Copyright 2020-2022 Paulo Marangoni.
// Use of this source code is governed by the license that can be found in the LICENSE file at the root of this distribution.

#pragma once

#include "Skore/Defines.hpp"
#include "Traits.hpp"
#include "Allocator.hpp"

namespace Skore
{

	template<typename Type>
	class PtrBase
	{
	public:
		using Base = Type;

		PtrBase() : m_Instance(nullptr), m_References(nullptr)
		{
		}

		PtrBase(Type* instance, i32* references) : m_Instance(instance), m_References(references)
		{
			*m_References = 1;
		}

		virtual ~PtrBase() noexcept
		{
			if (m_References != nullptr)
			{
				*this->m_References = *this->m_References - 1;
				if (*this->m_References <= 0)
				{
					Allocator* allocator = GetDefaultAllocator();
					m_Instance->~Type();
					allocator->MemFree(allocator->Alloc, m_Instance, sizeof(Traits::RemoveAll<Type>));
					allocator->MemFree(allocator->Alloc, m_References, sizeof(decltype(m_References)));
				}
			}
		}

		i32 RefCount() const
		{
			return *m_References;
		}

	protected:
		template<class Type2>
		void MoveConstructorFrom(PtrBase<Type2>&& right) noexcept
		{
			this->m_Instance   = right.m_Instance;
			this->m_References = right.m_References;

			right.m_References = nullptr;
			right.m_Instance   = nullptr;
		}

		template<class Type2>
		void CopyConstructorFrom(const PtrBase<Type2>& right)
		{
			this->m_Instance   = right.m_Instance;
			this->m_References = right.m_References;
			if (this->m_References != nullptr)
			{
				*this->m_References = *this->m_References + 1;
			}
		}

		template<class Type2>
		void AliasConstructFrom(const PtrBase<Type2>& right, Type* p_instance)
		{
			this->m_Instance   = p_instance;
			this->m_References = right.m_References;
			if (this->m_References != nullptr)
			{
				*this->m_References = *this->m_References + 1;
			}
		}

		template<class Type2, typename Alloc2>
		void AliasConstructFrom(PtrBase<Type2>&& right, Type* p_instance)
		{
			this->m_Instance   = p_instance;
			this->m_References = right.m_References;

			right.m_References = nullptr;
			right.m_Instance   = nullptr;
		}

	private:
		template<class Type0>
		friend
		class PtrBase;

		template<class Type0>
		friend
		class SharedPtr;

		Type* m_Instance;
		i32 * m_References;
	};

	template<typename Type>
	class SharedPtr : public PtrBase<Type>
	{
	public:

		SharedPtr() : PtrBase<Type>()
		{}

		SharedPtr(Traits::NullPtr) : PtrBase<Type>()
		{}

		explicit SharedPtr(Type* instance) : PtrBase<Type>(instance, Alloc<i32>())
		{}

		SharedPtr(Type* instance, TypeID typeId) : PtrBase<Type>(instance, Alloc<i32>(), typeId)
		{}

		SharedPtr(const SharedPtr& sharedPtr)
		{
			this->CopyConstructorFrom(sharedPtr);
		}

		SharedPtr(SharedPtr&& sharedPtr) noexcept
		{
			this->MoveConstructorFrom(Traits::Move(sharedPtr));
		}

		template<typename Type2>
		SharedPtr(const SharedPtr<Type2>& sharedPtr)
		{
			this->CopyConstructorFrom(sharedPtr);
		}

		template<typename Type2>
		SharedPtr(SharedPtr<Type2>&& sharedPtr) noexcept
		{
			this->MoveConstructorFrom(Traits::Move(sharedPtr));
		}

		template<typename Type2>
		SharedPtr(const SharedPtr<Type2>& sharedPtr, Type* ptr)
		{
			this->AliasConstructFrom(sharedPtr, ptr);
		}

		template<typename Type2>
		SharedPtr(SharedPtr<Type2>&& sharedPtr, Type* ptr) noexcept
		{
			this->AliasConstructFrom(Traits::Move(sharedPtr), ptr);
		}

		SharedPtr& operator=(const SharedPtr& sharedPtr) noexcept
		{
			this->CopyConstructorFrom(sharedPtr);
			return *this;
		}

		template<typename Type2>
		SharedPtr& operator=(const SharedPtr<Type2>& sharedPtr) noexcept
		{
			this->CopyConstructorFrom(sharedPtr);
			return *this;
		}

		bool operator==(Traits::NullPtr) const noexcept
		{
			return this->m_Instance == nullptr;
		}

		bool operator!=(Traits::NullPtr) noexcept
		{
			return this->m_Instance != nullptr;
		}

		Type* Get() const noexcept
		{
			return this->m_Instance;
		}

		Type& operator*() const noexcept
		{
			return *Get();
		}

		Type* operator->() const noexcept
		{
			return Get();
		}

		explicit operator bool() const noexcept
		{
			return this->m_Instance != nullptr;
		}
	};

	template<typename Type, typename ...Args>
	inline SharedPtr<Type> MakeShared(Args&& ... args)
	{
		return SharedPtr<Type>(Alloc<Type>(Traits::Forward<Args>(args)...));
	}
//
//    template<typename Type1, typename Type2>
//    inline SharedPtr<Type1> staticPointerCast(const SharedPtr<Type2>& sharedPtr) {
//        const auto ptr = static_cast<typename SharedPtr<Type1>::Base*>(sharedPtr.get());
//        return SharedPtr<Type1>(sharedPtr, ptr);
//    }
//
//    template<typename Type1, typename Type2>
//    inline SharedPtr<Type1> staticPointerCast(SharedPtr<Type2>&& sharedPtr) {
//        const auto ptr = static_cast<typename SharedPtr<Type1>::Base*>(sharedPtr.get());
//        return SharedPtr<Type1>(Traits::Move(sharedPtr), ptr);
//    }
}

