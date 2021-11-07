#pragma once
#include <vector>
#include <string>
#include <stdarg.h>
#include <filesystem>

class Logger
{
	enum class Severity {
		Info,
		Debug,
		Warning,
		Error
	};

	static Logger* logger;

public:
	void LogInfo(std::string);
	void LogDebug(std::string);
	void LogWarning(std::string);
	void LogError(std::string);

	static Logger* GetLogger() {
		if (!logger) {
			logger = new Logger;
		}
		return logger;
	}

private:
	Logger() {};
	unsigned GetSeverityMaxBufferCount(Severity);
	const char* GetSeverityId(Severity);
	const char* m_Name;
	const char* m_file_name = "Log.log";
	std::vector<std::string> Logger::m_Buffer;
	void Log(Severity severity, std::string);

public:
	void Init();
	void Flush();
	~Logger();
};

