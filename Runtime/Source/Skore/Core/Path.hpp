//  Copyright 2023 Paulo Marangoni.
//  Use of this source code is governed by the license that can be found in the LICENSE file at the root of this distribution.

#pragma once

#include "Skore/Core/String.hpp"
#include "Skore/Core/StringView.hpp"

namespace Skore
{

	template<typename Type>
	class BasePath
	{
	public:

		BasePath()
		{}

		BasePath(const String& string) : m_AbsolutePath(string)
		{
		}

		BasePath(const StringView& absolutePath) : m_AbsolutePath(absolutePath)
		{
		}

		BasePath(const char* str) : m_AbsolutePath(str)
		{
		}

		BasePath(const char* str, usize size) : m_AbsolutePath(str, size)
		{
		}

		BasePath(const BasePath& path) : m_AbsolutePath(path.m_AbsolutePath)
		{
		}


		bool Empty() const
		{
			return m_AbsolutePath.Empty();
		}

		decltype(auto) begin()
		{
			return m_AbsolutePath.begin();
		}

		decltype(auto) end()
		{
			return m_AbsolutePath.end();
		}

		decltype(auto) begin() const
		{
			return m_AbsolutePath.begin();
		}

		decltype(auto) end() const
		{
			return m_AbsolutePath.end();
		}

		decltype(auto) Parent() const
		{
			String parentPath{};
			bool foundSeparator = false;
			auto it = m_AbsolutePath.end();
			do
			{
				if (foundSeparator)
				{
					parentPath.Insert(parentPath.begin(), *it);
				}
				if (*it == SK_PATH_SEPARATOR)
				{
					foundSeparator = true;
				}

				if (it == m_AbsolutePath.begin())
				{
					return BasePath{parentPath};
				}
				it--;
			} while (true);
		}

		BasicStringView<Type> Extension() const
		{
			auto it = m_AbsolutePath.end();
			while (it != m_AbsolutePath.begin())
			{
				it--;
				if (*it == '.')
				{
					return BasicStringView<Type>{it, (usize) (m_AbsolutePath.end() - it)};
				}
				else if (*it == SK_PATH_SEPARATOR)
				{
					break;
				}
			}
			return {};
		}

		void Append(const StringView& stringView)
		{
			m_AbsolutePath.Append(stringView);
		}

		BasicString<Type> Name() const
		{
			bool hasExtension = !Extension().Empty();

			auto it = m_AbsolutePath.end();
			if (it == m_AbsolutePath.begin()) return {};
			it--;

			//if the last char is a separator
			//like /path/folder/
			if (*it == SK_PATH_SEPARATOR)
			{
				it--;
			}

			String name{};
			bool found{};

			if (!hasExtension)
			{
				found = true;
			}

			while (it != m_AbsolutePath.begin())
			{
				if (found && *it == SK_PATH_SEPARATOR)
				{
					return name;
				}
				if (found)
				{
					name.Insert(name.begin(), *it);
				}
				if (*it == '.' || *it == SK_PATH_SEPARATOR)
				{
					found = true;
				}
				it--;
			}
			return m_AbsolutePath;
		}

		const String& GetString() const
		{
			return m_AbsolutePath;
		}

		inline decltype(auto) operator+=(const StringView& b)
		{
			m_AbsolutePath.Append(SK_PATH_SEPARATOR);
			m_AbsolutePath.Append(b);
			return *this;
		}

	private:
		BasicString<Type> m_AbsolutePath{};
	};


	template<typename Type>
	inline decltype(auto) operator+(const BasePath<Type>& a, const StringView& b)
	{
		String path = a.GetString();
		if (!b.Empty())
		{
			if (b[0] != '.')
			{
				path.Append(SK_PATH_SEPARATOR);
			}

			path.Reserve(path.Size() + b.Size());

			for (int i = 0; i < b.Size(); ++i)
			{
				if (b[i] == '/' || b[i] == '\\')
				{
					path.Append(SK_PATH_SEPARATOR);
				}
				else
				{
					path.Append(b[i]);
				}
			}
		}
		return BasePath<Type>{path};
	}

	template<>
	struct StringConverter<BasePath<char>>
	{
		typedef char Element;

		constexpr static bool  HasConverter = true;
		constexpr static usize BufferCount  = 0;

		static usize Size(const BasicStringView<Element>& stringView)
		{
			return stringView.Size();
		}

		static usize ToString(char* buffer, usize pos, const BasePath<Element>& value)
		{
			StrCopy(buffer, pos, value.begin(), value.GetString().Size());
			return value.GetString().Size();
		}

		static void FromString(const char* str, usize size, BasePath<Element>& value)
		{
			value = {str, size};
		}
	};

	using Path = BasePath<char>;
}