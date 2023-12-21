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

		BasePath(const String& string) : m_absolutePath(string)
		{
		}

		BasePath(const StringView& absolutePath) : m_absolutePath(absolutePath)
		{
		}

		BasePath(const char* str) : m_absolutePath(str)
		{
		}

		BasePath(const char* str, usize size) : m_absolutePath(str, size)
		{
		}

		BasePath(const BasePath& path) : m_absolutePath(path.m_absolutePath)
		{
		}


		bool Empty() const
		{
			return m_absolutePath.Empty();
		}

		decltype(auto) begin()
		{
			return m_absolutePath.begin();
		}

		decltype(auto) end()
		{
			return m_absolutePath.end();
		}

		decltype(auto) begin() const
		{
			return m_absolutePath.begin();
		}

		decltype(auto) end() const
		{
			return m_absolutePath.end();
		}

		decltype(auto) Parent() const
		{
			String parentPath{};
			bool foundSeparator = false;
			auto it = m_absolutePath.end();
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

				if (it == m_absolutePath.begin())
				{
					return BasePath{parentPath};
				}
				it--;
			} while (true);
		}

		BasicStringView<Type> Extension() const
		{
			auto it = m_absolutePath.end();
			while (it != m_absolutePath.begin())
			{
				it--;
				if (*it == '.')
				{
					return BasicStringView<Type>{it, (usize) (m_absolutePath.end() - it)};
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
			m_absolutePath.Append(stringView);
		}

		BasicString<Type> Name() const
		{
			bool hasExtension = !Extension().Empty();

			auto it = m_absolutePath.end();
			if (it == m_absolutePath.begin()) return {};
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

			while (it != m_absolutePath.begin())
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
			return m_absolutePath;
		}

		const String& GetString() const
		{
			return m_absolutePath;
		}

		inline decltype(auto) operator+=(const StringView& b)
		{
			m_absolutePath.Append(SK_PATH_SEPARATOR);
			m_absolutePath.Append(b);
			return *this;
		}

	private:
		BasicString<Type> m_absolutePath{};
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