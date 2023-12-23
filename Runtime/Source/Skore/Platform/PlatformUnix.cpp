//   Copyright 2023 Paulo Marangoni.
//   Use of this source code is governed by the license that can be found in the LICENSE file at the root of this distribution.

#include "Skore/Platform/Platform.hpp"


#ifdef SK_UNIX

#include <dlfcn.h>
#include "Skore/IO/FileSystem.hpp"
#include "Skore/Core/Logger.hpp"

#include <ctime>
#include <cstdio>

namespace Skore
{

	namespace Platform
	{
		void ApplyDarkStyle(CPtr internal);
		void InitStyle();
		Logger linuxLogger = Logger::GetLogger("Skore::PlatformLinux");
	}

	void Platform::InitStyle()
	{
	}

	void Platform::ApplyDarkStyle(CPtr internal)
	{
	}

	f64 Platform::GetTime()
	{
		struct timespec now{};
		clock_gettime(CLOCK_MONOTONIC_RAW, &now);
		return now.tv_sec + now.tv_nsec * 0.000000001;
	}

    void Platform::ConsoleWrite(const StringView& message)
    {
        printf("%s", message.CStr());
    }
}


#endif