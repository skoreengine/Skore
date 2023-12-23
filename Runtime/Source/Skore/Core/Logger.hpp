// Copyright 2023 Paulo Marangoni.
// Use of this source code is governed by the license that can be found in the LICENSE file at the root of this distribution.

#pragma once

#include "Skore/Defines.hpp"
#include "StringView.hpp"
#include "String.hpp"
#include "Formatter.hpp"
namespace Skore
{
	enum LogLevel_
	{
		LogLevel_Trace    = 0,
		LogLevel_Debug    = 1,
		LogLevel_Info     = 2,
		LogLevel_Warn     = 3,
		LogLevel_Error    = 4,
		LogLevel_Critical = 5,
		LogLevel_Off      = 6
	};
	typedef u32 LogLevel;

	typedef u32 LogLevel;

	class SK_API Logger
	{
	public:

		static Logger& GetLogger(const StringView& name);
		static Logger& GetLogger(const StringView& name, LogLevel logLevel);

		Logger(const String& name, LogLevel logLevel);

		void SetLevel(LogLevel logLevel)
		{
			m_logLevel = logLevel;
		}

		LogLevel& GetLevel()
		{
			return m_logLevel;
		}

		bool CheckLevel(LogLevel level)
		{
			return level >= m_logLevel;
		}

		template<typename ...Args>
		inline void Log(LogLevel level, const StringView& message, Args&& ...args)
		{
			if (!CheckLevel(level)) return;

			char logBuffer[100];
			char* buffer;

			auto size          = Formatter::FormattedSize(message, Traits::Forward<Args>(args)...);
			bool heapAllocated = size >= sizeof(logBuffer);
			if (heapAllocated)
			{
				buffer = (char*) GetDefaultAllocator()->MemAlloc(GetDefaultAllocator()->Alloc, size + 1);
			}
			else
			{
				buffer = logBuffer;
			}
			buffer[size] = '\0';

			Formatter::FormatTo(message, buffer, Traits::Forward<Args>(args)...);

			PrintLog(level, StringView(buffer, size));

			if (heapAllocated)
			{
				GetDefaultAllocator()->MemFree(GetDefaultAllocator()->Alloc, buffer, size);
			}
		}

		template<typename ...Args>
		inline void Trace(const StringView& message, Args&& ...args)
		{
			Log(LogLevel_Trace, message, Traits::Forward<Args>(args)...);
		}

		template<typename ...Args>
		inline void Debug(const StringView& message, Args&& ...args)
		{
			Log(LogLevel_Debug, message, Traits::Forward<Args>(args)...);
		}

		template<typename ...Args>
		inline void Info(const StringView& message, Args&& ...args)
		{
			Log(LogLevel_Info, message, Traits::Forward<Args>(args)...);
		}

		template<typename ...Args>
		inline void Warn(const StringView& message, Args&& ...args)
		{
			Log(LogLevel_Warn, message, Traits::Forward<Args>(args)...);
		}

		template<typename ...Args>
		inline void Error(const StringView& message, Args&& ...args)
		{
			Log(LogLevel_Error, message, Traits::Forward<Args>(args)...);
		}

		template<typename ...Args>
		inline void Critical(const StringView& message, Args&& ...args)
		{
			Log(LogLevel_Critical, message, Traits::Forward<Args>(args)...);
		}

	private:
		String   m_name{};
		LogLevel m_logLevel{};
		void PrintLog(LogLevel logLevel, const StringView& message);
	};

}