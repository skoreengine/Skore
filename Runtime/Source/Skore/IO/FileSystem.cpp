// Copyright 2023 Paulo Marangoni.
// Use of this source code is governed by the license that can be found in the LICENSE file at the root of this distribution.


#include "FileSystem.hpp"
#include <filesystem>
namespace fs = std::filesystem;

namespace Skore::FileSystem
{

	bool Remove(const StringView& path)
	{
		return fs::remove_all(path.CStr());
	}

	bool CreateDirectory(const StringView& path)
	{
		return fs::create_directories(fs::path(path.begin(), path.end()));
	}

	bool Rename(const StringView& newName, const StringView& oldName)
	{
		std::error_code ec{};
		fs::rename(oldName.CStr(), newName.CStr(), ec);
		return ec.value() == 0;
	}

	bool CopyFile(const StringView& from, const StringView& to)
	{
		auto toPath = fs::path(to.begin(), to.end());
		const auto copyOptions = fs::copy_options::update_existing | fs::copy_options::recursive;
		std::error_code ec{};
		fs::copy(fs::path(from.begin(), from.end()), toPath, copyOptions, ec);
		return ec.value() == 0;
	}

}