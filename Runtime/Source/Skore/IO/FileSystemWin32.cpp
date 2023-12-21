//Copyright 2023 Paulo Marangoni.
//Use of this source code is governed by the license that can be found in the LICENSE file at the root of this distribution.

#include "FileSystem.hpp"

#ifdef SK_WIN

#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <direct.h>
#include <KnownFolders.h>
#include <ShlObj.h>
#include <shellapi.h>
#include <cstdio>

#ifdef CreateDirectory
#undef CreateDirectory
#endif

#ifdef CopyFile
#undef CopyFile
#endif

namespace Skore
{
	DirIterator& DirIterator::operator++()
	{
		if (m_handler)
		{
			WIN32_FIND_DATA fd{};
			auto res = FindNextFile(m_handler, &fd);
			if (res != 0)
			{
				do
				{
					bool isDirEntry = !(strcmp(fd.cFileName, ".") == 0 || strcmp(fd.cFileName, "..") == 0);
					if (isDirEntry)
					{
						m_path = m_directory + fd.cFileName;
						return *this;
					}
				} while (::FindNextFile(m_handler, &fd) != 0);
			}
			FindClose(m_handler);
			m_handler = nullptr;
		}
		m_path = {};
		return *this;
	}

	DirIterator::DirIterator(const Path& directory) : m_directory(directory), m_handler(nullptr)
	{
		WIN32_FIND_DATA fd{};
		char cwd[MAX_PATH];
		sprintf(cwd, "%s\\*.*", directory.GetString().CStr());
		m_handler = FindFirstFile(cwd, &fd);

		if (m_handler != INVALID_HANDLE_VALUE)
		{
			do
			{
				bool isDirEntry = !(strcmp(fd.cFileName, ".") == 0 || strcmp(fd.cFileName, "..") == 0);
				if (isDirEntry)
				{
					m_path = directory + fd.cFileName;
					break;
				}
			} while (::FindNextFile(m_handler, &fd) != 0);
		}
	}

	DirIterator::~DirIterator()
	{
		if (m_handler)
		{
			FindClose(m_handler);
		}
	}
}


namespace Skore::FileSystem
{

	Path CurrentDir()
	{
		TCHAR path[MAX_PATH];
		GetCurrentDirectory(MAX_PATH, path);
		return {path, strlen(path)};
	}

	Path DocumentsDir()
	{
		CHAR myDocuments[MAX_PATH];
		HRESULT result = SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, myDocuments);
		return {myDocuments};
	}

	Skore::FileStatus FileStatus(const StringView& path)
	{
		WIN32_FILE_ATTRIBUTE_DATA fileAttrData = {0};
		bool exists = GetFileAttributesEx(path.CStr(), GetFileExInfoStandard, &fileAttrData);
		return {exists, (fileAttrData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0,
		        (u64) (static_cast<i64>(fileAttrData.ftLastWriteTime.dwHighDateTime) << 32 | fileAttrData.ftLastWriteTime.dwLowDateTime)};
	}
	
	Path AppFolder()
	{
		PWSTR pathTemp;
		SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, nullptr, &pathTemp);
		char buffer[MAX_PATH];
		usize size{};
		wcstombs_s(&size, buffer, pathTemp, MAX_PATH);
		CoTaskMemFree(pathTemp);
		return {buffer, size - 1};
	}

	FileResult LoadFile(const StringView& path, Array<u8>& buffer)
	{
		HANDLE hout = CreateFile(path.CStr(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
		if (hout == INVALID_HANDLE_VALUE)
		{
			return FileResult_Error;
		}

		LARGE_INTEGER size;
		if (!GetFileSizeEx(hout, &size))
		{
			return FileResult_Error;
		}
		buffer.Resize(size.QuadPart);

		DWORD nRead;
		ReadFile(hout, buffer.Data(), size.QuadPart, &nRead, nullptr);
		CloseHandle(hout);
		return FileResult_Success;
	}

	FileResult SaveFile(const StringView& path, const Span<u8>& buffer)
	{
		HANDLE hin = CreateFile(path.CStr(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
		if (hin == INVALID_HANDLE_VALUE)
		{
			return FileResult_Error;
		}

		DWORD nWritten;
		WriteFile(hin, buffer.Data(), buffer.Size(), &nWritten, nullptr);
		CloseHandle(hin);
		return FileResult_Success;
	}

	FileResult LoadTextFile(const StringView& path, String& buffer)
	{
		HANDLE hout = CreateFile(path.CStr(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
		if (hout == INVALID_HANDLE_VALUE)
		{
			return FileResult_Error;
		}

		LARGE_INTEGER size;
		if (!GetFileSizeEx(hout, &size))
		{
			return FileResult_Error;
		}
		buffer.Resize(size.QuadPart);

		DWORD nRead;
		ReadFile(hout, buffer.begin(), size.QuadPart, &nRead, nullptr);
		CloseHandle(hout);
		return FileResult_Success;
	}

}

#endif