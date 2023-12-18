// Copyright 2023 Paulo Marangoni.
// Use of this source code is governed by the license that can be found in the LICENSE file at the root of this distribution.

#pragma once

#include "Traits.hpp"
#include "SharedPtr.hpp"

namespace Skore
{

	template<typename Ret, typename...Params>
	class FunctionBase
	{
	public:
		virtual Ret Call(Params...) = 0;
		virtual ~FunctionBase() = default;
	};

	template<typename Func, typename Ret, typename...Params>
	class FunctionBaseImpl : public FunctionBase<Ret, Params...>
	{
	private:
		Func m_func;
	public:
		FunctionBaseImpl(Func func) : m_func(func)
		{}

		Ret Call(Params ...params) override
		{
			return m_func(Traits::Forward<Params>(params)...);
		}
	};

	template<typename Ret, typename...Params>
	class FunctionImpl
	{
	public:
		Ret operator()(Params... params)
		{
			return m_funcBase->Call(Traits::Forward<Params>(params)...);
		}

		bool IsEmpty() const
		{
			return m_funcBase == nullptr;
		}

	protected:
		template<typename Func>
		void Set(Func&& func)
		{
			m_funcBase = MakeShared<FunctionBaseImpl<Func, Ret, Params...>>(func);
		}

	private:
		SharedPtr<FunctionBase<Ret, Params...>> m_funcBase{};
	};


	template<typename T>
	struct FunctionDecomp
	{
		static_assert(Traits::AlwaysFalse<T>, "Function only accept function types");
	};


	template<typename Ret, typename...Params>
	struct FunctionDecomp<Ret(Params...)>
	{
		using type = FunctionImpl<Ret, Params...>;
	};


	template<typename Type>
	class Function : public FunctionDecomp<Type>::type
	{
	public:
		Function() noexcept {}
		Function(Traits::NullPtr) noexcept {}

		template<typename Func>
		Function(Func&& func)
		{
			this->Set(func);
		}

		operator bool() const noexcept
		{
			return !this->IsEmpty();
		}
	};


}