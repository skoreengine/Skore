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
			return m_path;
		}

		Path* operator->()
		{
			return &m_path;
		}

		friend bool operator==(const DirIterator& a, const DirIterator& b)
		{
			return a.m_path.GetString() == b.m_path.GetString();
		};

		friend bool operator!=(const DirIterator& a, const DirIterator& b)
		{
			return a.m_path.GetString() != b.m_path.GetString();
		};

		DirIterator& operator++();

		virtual ~DirIterator();

	private:
		Path m_directory{};
		Path m_path{};
		CPtr m_handler{};
	};

	class SK_API DirectoryEntries
	{
	public:

		DirectoryEntries(const Path& directory) : m_directory(directory)
		{}

		DirIterator begin()
		{
			return DirIterator{m_directory};
		}

		DirIterator end()
		{
			return DirIterator{};
		}

	private:
		Path m_directory{};
	};

	struct FileStatus
	{
		bool exists{};
		bool isDirectory{};
		u64  lastModifiedTime{};
	};

	enum FileResult_
	{
		FileResult_Success = 0,
		FileResult_Error   = 1
	};

	typedef u32 FileResult;

}