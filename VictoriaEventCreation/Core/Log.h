#pragma once

#define RE_LogMessage(message) Log::LogMessage("Log: ",message)
#define RE_LogWarning(message) Log::LogMessage("Warning: ",message)
#define RE_LogError(message) Log::LogMessage("Error: ",message)

struct LogEntry
{
	std::string type;
	std::string log;
};

class Log
{
public:
	static void LogMessage(const std::string& type,const std::string& log);
	static void SaveLogs();
	static std::vector<LogEntry> logs;
};

