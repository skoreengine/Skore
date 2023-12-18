// Copyright 2023 Paulo Marangoni.
// Use of this source code is governed by the license that can be found in the LICENSE file at the root of this distribution.

#pragma once

#include "StringView.hpp"
#include "Skore/Defines.hpp"
#include "Traits.hpp"
#include "Algorithm.hpp"
#include "Function.hpp"
#include "StringConverter.hpp"

namespace Skore::Formatter
{
	template<typename Arg>
	constexpr void MakeConvertFunc(Function<usize(char* buffer, usize pos)>& func, const Arg& arg)
	{
		func = [&](char* buffer, usize pos)
		{
			return StringConverter<Traits::RemoveAll<Arg>>::ToString(buffer, pos, arg);
		};
	}

	template<typename Arg>
	constexpr void MakeSizeFunc(Function<usize()>& func, Arg&& arg)
	{
		func = [&]()
		{
			return StringConverter<Traits::RemoveAll<Arg>>::Size(arg);
		};
	}

	template<typename ...Args>
	struct FormatterArgs
	{
		Function<usize(char* buffer, usize pos)> convertFunc[sizeof...(Args)];
		Function<usize()> sizeFunc[sizeof...(Args)];

		explicit FormatterArgs(Args&& ... args)
		{
			{
				usize i{};
				(MakeConvertFunc(convertFunc[i++], Traits::Forward<Args>(args)), ...);
			}
			{
				usize i{};
				(MakeSizeFunc(sizeFunc[i++], Traits::Forward<Args>(args)), ...);
			}

		}

		usize Format(usize index, char* buffer, usize pos)
		{
			if (index < sizeof...(Args))
			{
				return convertFunc[index](buffer, pos);
			}
			return 0;
		}

		usize Size(usize index)
		{
			if (index < sizeof...(Args))
			{
				return sizeFunc[index]();
			}
			return 0;
		}
	};

	template<typename ...Args>
	inline usize FormattedSize(const StringView& fmtString, Args&& ... args)
	{
		auto  formatter = FormatterArgs<Args...>(Traits::Forward<Args>(args)...);
		usize j         = 0;
		usize p         = 0;

		for (int i = 0; i < fmtString.Size(); ++i)
		{
			if (fmtString.Size() - 1 > i && fmtString[i] == '{' && fmtString[i + 1] == '}')
			{
				j += formatter.Size(p);
				p++;
				i++;
			}
			else
			{
				j++;
			};
		}
		return j;
	}

	inline usize FormattedSize(const StringView& fmtString)
	{
		return fmtString.Size();
	}

	template<typename ...Args>
	inline void FormatTo(const StringView& fmtString, char* output, Args&& ... args)
	{
		auto formatter = FormatterArgs<Args...>(Traits::Forward<Args>(args)...);

		usize j = 0;
		usize p = 0;

		for (int i = 0; i < fmtString.Size(); ++i)
		{
			if (fmtString.Size() - 1 > i && fmtString[i] == '{' && fmtString[i + 1] == '}')
			{
				j += formatter.Format(p, output, j);
				p++;
				i++;
			}
			else
			{
				output[j++] = fmtString[i];
			};
		}
	}

	inline void FormatTo(const StringView& fmtString, char* output)
	{
		StrCopy(output, fmtString.CStr(), fmtString.Size());
	}
}