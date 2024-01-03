#include "Skore/Platform/Platform.hpp"
#include "Skore/Platform/PlatformStyle.hpp"


#ifdef SK_UNIX
#include <ctime>
#include <dlfcn.h>
#include <cstdio>
#include "Skore/IO/FileSystem.hpp"
#include "Skore/Core/Logger.hpp"

namespace Skore
{

    struct PlatformUnix
    {
        Logger& Logger = Logger::GetLogger("Skore::Platform");
    };

    PlatformUnix platformUnix;

    void PlatformStyle::Init()
    {

    }

    void PlatformStyle::ApplyDarkStyle(CPtr ptr)
    {

    }


    f64 Platform::GetTime()
    {
        struct timespec now{};
        clock_gettime(CLOCK_MONOTONIC_RAW, &now);
        return now.tv_sec + now.tv_nsec * 0.000000001;
    }

    CPtr Platform::LoadSharedLib(const StringView& library)
    {
        CPtr ptr;
        if (FileSystem::FileStatus(library).Exists)
        {
            ptr = dlopen(library.CStr(), RTLD_NOW);
        }
        else
        {
            String file = library;
            file += SK_SHARED_EXT;
            ptr = dlopen(file.CStr(), RTLD_NOW);
        }
        if (!ptr)
        {
            const char* err = dlerror();
            platformUnix.Logger.Error(" Cannot open {}", err);
        }
        return ptr;
    }

    void Platform::FreeSharedLib(CPtr library)
    {
        dlclose(library);
    }

    CPtr Platform::GetAddress(CPtr library, const char *funcName)
    {
        return dlsym(library, funcName);
    }

    void Platform::ConsoleWrite(const StringView &message)
    {
        printf("%s", message.CStr());
    }

}

#endif