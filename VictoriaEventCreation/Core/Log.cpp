#include "pch.h"
#include "Log.h"
#include "FilesysHelper.h"

std::vector<LogEntry> Log::logs;

void Log::LogMessage(const std::string& type, const std::string& log)
{
    static std::mutex logsMutex;
    std::lock_guard lock(logsMutex);
    logs.emplace_back(type,log);
}

void Log::SaveLogs()
{
    auto fileLocation = std::filesystem::current_path() / "log.txt";
    auto file = BaseApp::CreateFile(fileLocation);
    if (file)
    {
        for (const auto& log : logs)
        {
            file << log.type << log.log << std::endl;
        }
        file << "Log Saved.";
        file.close();
    }
}