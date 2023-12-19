// Copyright 2023 Paulo Marangoni.
// Use of this source code is governed by the license that can be found in the LICENSE file at the root of this distribution.

#include <mutex>
#include "Logger.hpp"
#include "HashMap.hpp"
#include "Skore/Platform/Platform.hpp"

namespace Skore
{
	const char* levelDesc[] = {"Trace", "Debug", "Info", "Warn", "Error", "Critical", "Off"};

	struct LogContext
	{
		String                             buffer{4096};
		HashMap<String, SharedPtr<Logger>> loggers{};
		std::mutex                         mutex{};
	};

	LogContext& GetLoggerContext()
	{
		static LogContext logContext{};
		return logContext;
	}

	Logger::Logger(const String& name, LogLevel logLevel) : m_name(name), m_logLevel(logLevel)
	{
	}

	Logger& Logger::GetLogger(const StringView& name)
	{
		return GetLogger(name, LogLevel_Info);
	}

	Logger& Logger::GetLogger(const StringView& name, LogLevel logLevel)
	{
		auto it = GetLoggerContext().loggers.Find(name);
		if (it == GetLoggerContext().loggers.end())
		{
			it = GetLoggerContext().loggers.Insert(MakePair(String{name}, MakeShared<Logger>(name, logLevel))).first;
		}
		return *it->second;
	}

	void Logger::PrintLog(LogLevel logLevel, const StringView& message)
	{
		std::unique_lock<std::mutex> lock(GetLoggerContext().mutex);
		const auto  now = std::chrono::system_clock::now();
		std::time_t t   = std::chrono::system_clock::to_time_t(now);

		tm buf;
		gmtime_s(&buf, &t);

		char buffer[50];
		strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &buf);

		auto duration     = now.time_since_epoch();

		i64 milliseconds = (std::chrono::duration_cast<std::chrono::milliseconds>(duration) -
			duration_cast<std::chrono::milliseconds>(std::chrono::duration_cast<std::chrono::seconds>(duration))).count();

		LogContext& context = GetLoggerContext();
		context.buffer.Clear();

		context.buffer = "[";
		context.buffer += buffer;
		context.buffer += ".";
		if (milliseconds < 100)
		{
			context.buffer += "0";
		}
		if (milliseconds < 10)
		{
			context.buffer += "0";
		}
		usize size = StringConverter<i64>::ToString(context.buffer.begin(), context.buffer.Size(), milliseconds);
		context.buffer.SetSize(context.buffer.Size() + size);
		context.buffer += "] [";
		context.buffer += levelDesc[logLevel];
		context.buffer += "] [";
		context.buffer += m_name;
		context.buffer += "] ";
		context.buffer += message;
		context.buffer += "\n";
		Platform::ConsoleWrite(context.buffer);
	}
}