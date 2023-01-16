#pragma once

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

