//Copyright 2023 Paulo Marangoni.
//Use of this source code is governed by the license that can be found in the LICENSE file at the root of this distribution.

#pragma once

#include <Skore/Core/Array.hpp>
#include "Skore/Defines.hpp"
#include "FileTypes.hpp"
#include "Skore/Core/Span.hpp"

namespace Skore::FileSystem
{

	SK_API Path       CurrentDir();
	SK_API Path       DocumentsDir();
	SK_API FileStatus FileStatus(const StringView &path);
	SK_API bool       CreateDirectory(const StringView &path);
	SK_API bool       Remove(const StringView &path);
	SK_API bool       Rename(const StringView &newName, const StringView &oldName);
	SK_API bool       CopyFile(const StringView &from, const StringView &to);
	SK_API Path       AppFolder();

	SK_API FileResult LoadFile(const StringView &path, Array<u8>& buffer);
	SK_API FileResult LoadTextFile(const StringView &path, String& buffer);
	SK_API FileResult SaveFile(const StringView &path, const Span<u8>& buffer);
}