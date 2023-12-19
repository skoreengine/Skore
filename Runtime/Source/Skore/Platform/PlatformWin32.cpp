// Copyright 2023 Paulo Marangoni.
// Use of this source code is governed by the license that can be found in the LICENSE file at the root of this distribution.

#include "Skore/Defines.hpp"

#ifdef SK_WIN
#include "Platform.hpp"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>


namespace Skore
{
	static f64 clockFrequency{};
	static LARGE_INTEGER startTime{};

	void ClockSetup()
	{
		LARGE_INTEGER frequency;
		QueryPerformanceFrequency(&frequency);
		clockFrequency = 1.0 / (f64)frequency.QuadPart;
		QueryPerformanceCounter(&startTime);
	}

	f64 Platform::GetTime()
	{
		if (clockFrequency == 0)
		{
			ClockSetup();
		}

		LARGE_INTEGER nowTime;
		QueryPerformanceCounter(&nowTime);
		return (f64) nowTime.QuadPart * clockFrequency;
	}

	void Platform::ConsoleWrite(const StringView& message)
	{
		DWORD numberWritten = 0;
		WriteConsoleA(GetStdHandle(STD_OUTPUT_HANDLE),
			message.Data(),
			(DWORD)message.Size(),
			&numberWritten, 0);
	}
}


#endif