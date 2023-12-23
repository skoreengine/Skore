//  Copyright 2023 Paulo Marangoni.
//  Use of this source code is governed by the license that can be found in the LICENSE file at the root of this distribution.

#pragma once

#include "Skore/Defines.hpp"
#include "Random.hpp"
#include "Algorithm.hpp"
#include "StringConverter.hpp"
#include "StringView.hpp"


namespace Skore
{

	struct SK_API UUID
	{

		u64 FirstValue;
		u64 SecondValue;

		constexpr UUID() : FirstValue(0), SecondValue(0)
		{}

		constexpr UUID(u64 firstValue, u64 secondValue) : FirstValue(firstValue), SecondValue(secondValue)
		{}

		explicit operator bool() const noexcept
		{
			return this->FirstValue != 0 || this->SecondValue != 0;
		}

		static constexpr UUID FromName(const char* string)
		{
			u64 values[2] = {};
			usize len = 0;
			for (const char* it = string; *it; ++it)
			{
				++len;
			}
			MurmurHash3X64128(string, len, HashSeed32, values);
			return UUID{values[0], values[1]};
		}


		static inline UUID RandomUUID()
		{
			return {Random::Xorshift64star(), Random::Xorshift64star()};
		}

		static inline UUID FromString(const StringView& str)
		{

			if (str.Empty())
			{
				return UUID{};
			}
			auto uuid = UUID{};

			u32 count{};
			Split(str, StringView {"-"}, [&](const StringView& value)
			{
				switch (count)
				{
					case 0 :
					{
						uuid.FirstValue = HexTo64(value);
						uuid.FirstValue <<= 16;
						break;
					}
					case 1 :
					{
						uuid.FirstValue |= HexTo64(value);
						uuid.FirstValue <<= 16;
						break;
					}
					case 2 :
					{
						uuid.FirstValue |= HexTo64(value);
						break;
					}
					case 3 :
					{
						uuid.SecondValue = HexTo64(value);
						uuid.SecondValue <<= 48;
						break;
					}
					case 4 :
					{
						uuid.SecondValue |= HexTo64(value);
						break;
					}
					default:
					{
					}
				}
				count++;
			});
			return uuid;
		};

		inline bool operator==(const UUID& uuid) const
		{
			return this->FirstValue == uuid.FirstValue && this->SecondValue == uuid.SecondValue;
		}

		inline bool operator!=(const UUID& uuid) const
		{
			return !((*this) == uuid);
		}

		inline UUID& operator=(const UUID& uuid) = default;

	};

	template<>
	struct Hash<UUID>
	{
		constexpr static bool HasHash = true;
		constexpr static usize Value(const UUID& uuid)
		{
			auto result = (usize) (uuid.FirstValue ^ (uuid.FirstValue >> 32));
			result = 31 * result + (i32) (uuid.SecondValue ^ (uuid.SecondValue >> 32));
			return result;
		}
	};

	template<>
	struct StringConverter<UUID>
	{
		constexpr static bool  HasConverter = true;
		constexpr static usize BufferCount  = 36;
		constexpr static char  Digits[]     = "0123456789abcdef";
		constexpr static i32   Base         = 16;

		static usize Size(const UUID& value)
		{
			return BufferCount;
		}

		static void FromString(const char* str, usize size, UUID& value)
		{
			value = UUID::FromString(StringView{str, size});
		}

		static usize ToString(char* buffer, usize pos, const UUID& value)
		{
			auto firstValue = value.FirstValue;
			i32 i = 17;
			do
			{
				if (i != 8 && i != 13)
				{
					buffer[pos + i] = Digits[firstValue % Base];
					firstValue = firstValue / Base;
				}
				else
				{
					buffer[pos + i] = '-';
				}
				i--;
			} while (i > -1);
			buffer[pos + 18] = '-';
			auto secondValue = value.SecondValue;
			i = 35;
			do
			{
				if (i != 23)
				{
					buffer[pos + i] = Digits[secondValue % Base];
					secondValue = secondValue / Base;
				}
				else
				{
					buffer[pos + i] = '-';
				}
				i--;
			} while (i > 18);
			return BufferCount;
		}
	};

}