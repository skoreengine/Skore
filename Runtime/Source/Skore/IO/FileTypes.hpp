// Copyright 2023 Paulo Marangoni.
// Use of this source code is governed by the license that can be found in the LICENSE file at the root of this distribution.

#pragma once

#include <Skore/Core/Path.hpp>

namespace Skore
{
	class SK_API DirIterator
	{
	public:
		DirIterator() = default;

		explicit DirIterator(const Path& directory);

		Path& operator*()
		{
			return m_Path;
		}

		Path* operator->()
		{
			return &m_Path;
		}

		friend bool operator==(const DirIterator& a, const DirIterator& b)
		{
			return a.m_Path.GetString() == b.m_Path.GetString();
		};

		friend bool operator!=(const DirIterator& a, const DirIterator& b)
		{
			return a.m_Path.GetString() != b.m_Path.GetString();
		};

		DirIterator& operator++();

		virtual ~DirIterator();

	private:
		Path m_Directory{};
		Path m_Path{};
		CPtr m_Handler{};
	};

	class SK_API DirectoryEntries
	{
	public:

		DirectoryEntries(const Path& directory) : m_Directory(directory)
		{}

		DirIterator begin()
		{
			return DirIterator{m_Directory};
		}

		DirIterator end()
		{
			return DirIterator{};
		}

	private:
		Path m_Directory{};
	};

	struct FileStatus
	{
		bool Exists{};
		bool IsDirectory{};
		u64  LastModifiedTime{};
	};

	enum FileResult_
	{
		FileResult_Success = 0,
		FileResult_Error   = 1
	};

	typedef u32 FileResult;

}