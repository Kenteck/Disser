#include "Logger.h"

Logger* Logger::logger;

void Logger::Init() {
	if (std::filesystem::exists(Logger::m_file_name))
		std::filesystem::remove(Logger::m_file_name);
}

void Logger::Flush() {
	FILE* file = fopen(Logger::m_file_name, "a");
	if (file) {
		for (auto message : Logger::m_Buffer) {
			fwrite(message.c_str(), sizeof(char), message.length(), file);
		}
		fclose(file);
		Logger::m_Buffer.clear();
	}
}

unsigned Logger::GetSeverityMaxBufferCount(Severity severity) {
	switch (severity) {
	case Severity::Debug:
		return 50;
	case Severity::Warning:
		return 10;
	case Severity::Error:
		return 0;
	case Severity::Info:
	default:
		return 100;
	}
}

const char* Logger::GetSeverityId(Severity severity) {
	switch (severity) {
	case Severity::Debug:
		return "Debug";
	case Severity::Warning:
		return "Warning";
	case Severity::Error:
		return "Error";
	case Severity::Info:
	default:
		return "Info";
	}
}

void Logger::LogInfo(std::string str) {
	Logger::Log(Logger::Severity::Info, str);
}

void Logger::LogDebug(std::string str) {
	Logger::Log(Logger::Severity::Debug, str);
}

void Logger::LogWarning(std::string str) {
	Logger::Log(Logger::Severity::Warning, str);
}

void Logger::LogError(std::string str) {
	Logger::Log(Logger::Severity::Error, str);
}

void Logger::Log(Severity severity, std::string report) {
	std::string logMsg = "";

	constexpr uint32_t timeBufferSize = 16;
	std::time_t currentTime = std::time(nullptr);
	char timeBuffer[timeBufferSize];

	if (std::strftime(timeBuffer, timeBufferSize, "[%H:%M:%S]", std::localtime(&currentTime))) {
		logMsg += timeBuffer;
	}

	logMsg += " " + std::string(Logger::GetSeverityId(severity)) + ": " + report + "\n";

	Logger::m_Buffer.push_back(logMsg);

	if (Logger::m_Buffer.size() > Logger::GetSeverityMaxBufferCount(severity)) {
		Logger::Flush();
	}
}

Logger::~Logger() {

	Logger::Flush();
}