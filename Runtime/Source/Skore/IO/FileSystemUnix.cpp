//Copyright 2023 Paulo Marangoni.
//Use of this source code is governed by the license that can be found in the LICENSE file at the root of this distribution.

#include "FileSystem.hpp"

#ifdef SK_UNIX

#include <dirent.h>
#include <cstring>
#include <unistd.h>
#include <sys/stat.h>
#include <pwd.h>
#include <fcntl.h>

namespace Skore
{
	DirIterator& DirIterator::operator++()
	{
		if (handler)
		{
			auto dir = (DIR*) handler;
			struct dirent* entry;
			while ((entry = readdir(dir)) != nullptr)
			{
				bool isDirEntry = !(strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0);
				if (isDirEntry)
				{
					path = directory + entry->d_name;
					handler = dir;
					return *this;
				}
			}
			closedir((DIR*)handler);
			handler = nullptr;
		}
		path = {};
		return *this;
	}

	DirIterator::DirIterator(const Path& directory) : directory(directory), handler(nullptr)
	{
		auto dir = opendir(directory.GetString().CStr());
		if (dir)
		{
			struct dirent* entry;
			while ((entry = readdir(dir)) != nullptr)
			{
				bool isDirEntry = !(strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0);
				if (isDirEntry)
				{
					path = directory + entry->d_name;
					handler = dir;
					break;
				}
			}
		}
	}

	DirIterator::~DirIterator()
	{
		if (handler)
		{
			closedir((DIR*)handler);
		}
	}
}

namespace Skore::FileSystem
{
	Path CurrentDir()
	{
		char cwd[PATH_MAX];
		if (getcwd(cwd, sizeof(cwd)) != nullptr) {
			return {cwd, strlen(cwd)};
		}
		return {};
	}

	Path DocumentsDir()
	{
		return AppFolder();
	}

	struct FileStatus FileStatus(const StringView& path)
	{
		struct stat st{};
		bool exists = stat(path.CStr(), &st) == 0;
		return {
			.exists = exists,
			.isDirectory = S_ISDIR(st.st_mode),
			.lastModifiedTime = static_cast<f64>(st.st_mtime)
		};
	}

	Path AppFolder()
	{
		struct passwd *pw = getpwuid(getuid());
		const char *homedir = pw->pw_dir;
		return {homedir, strlen(homedir)};
	}


	FileResult LoadFile(const StringView& path, Vector<u8>& buffer)
	{
		struct stat st{};
		stat(path.CStr(), &st);
		buffer.Resize(st.st_size);

		int res = open(path.CStr(), O_RDONLY);
		if (res == -1)
		{
			return FileResult_Error;
		}

		auto size = read(res, buffer.Data(), buffer.Size());
		if (size < 0 ||  size != st.st_size)
		{
			return FileResult_Error;
		}

		close(res);

		return FileResult_Success;
	}

	FileResult SaveFile(const StringView& path, const Span<u8>& buffer)
	{
		int const out = open(path.CStr(), O_WRONLY | O_CREAT, S_IWRITE | S_IREAD);
		if (out == -1)
		{
			return FileResult_Error;
		}

		auto size = write(out, buffer.Data(), buffer.Size());
		if (size < 0)
		{
			return FileResult_Error;
		}
		close( out );
		return FileResult_Success;
	}

	FileResult LoadTextFile(const StringView& path, String& buffer)
	{
		struct stat st{};
		stat(path.CStr(), &st);
		buffer.Resize(st.st_size);

		int res = open(path.CStr(), O_RDONLY);
		if (res == -1)
		{
			return FileResult_Error;
		}

		auto size = read(res, buffer.begin(), buffer.Size());
		if (size < 0 ||  size != st.st_size)
		{
			return FileResult_Error;
		}

		close(res);

		return FileResult_Success;
	}
}

#endif