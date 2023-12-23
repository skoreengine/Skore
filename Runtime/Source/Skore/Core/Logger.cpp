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
		String                             Buffer{4096};
		HashMap<String, SharedPtr<Logger>> Loggers{};
		std::mutex                         Mutex{};
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
		auto it = GetLoggerContext().Loggers.Find(name);
		if (it == GetLoggerContext().Loggers.end())
		{
			it = GetLoggerContext().Loggers.Insert(MakePair(String{name}, MakeShared<Logger>(name, logLevel))).First;
		}
		return *it->Second;
	}

	void Logger::PrintLog(LogLevel logLevel, const StringView& message)
	{
		std::unique_lock<std::mutex> lock(GetLoggerContext().Mutex);
		const auto  now = std::chrono::system_clock::now();
		std::time_t t   = std::chrono::system_clock::to_time_t(now);

		tm* buf = localtime(&t);

		char buffer[50];
		strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", buf);

		auto duration     = now.time_since_epoch();

		i64 milliseconds = (std::chrono::duration_cast<std::chrono::milliseconds>(duration) -
			duration_cast<std::chrono::milliseconds>(std::chrono::duration_cast<std::chrono::seconds>(duration))).count();

		LogContext& context = GetLoggerContext();
		context.Buffer.Clear();

		context.Buffer = "[";
		context.Buffer += buffer;
		context.Buffer += ".";
		if (milliseconds < 100)
		{
			context.Buffer += "0";
		}
		if (milliseconds < 10)
		{
			context.Buffer += "0";
		}
		usize size = StringConverter<i64>::ToString(context.Buffer.begin(), context.Buffer.Size(), milliseconds);
		context.Buffer.SetSize(context.Buffer.Size() + size);
		context.Buffer += "] [";
		context.Buffer += levelDesc[logLevel];
		context.Buffer += "] [";
		context.Buffer += m_name;
		context.Buffer += "] ";
		context.Buffer += message;
		context.Buffer += "\n";
		Platform::ConsoleWrite(context.Buffer);
	}
}