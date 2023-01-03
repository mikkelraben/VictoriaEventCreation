#include "pch.h"
#include "FilesysHelper.h"

void BaseApp::CreateDirectory(const std::filesystem::path& directory)
{
    if (!std::filesystem::exists(directory))
    {
        RE_LogWarning((directory.string() + " directory does not exist").c_str());
        RE_LogMessage("Creating directory");
        std::filesystem::create_directories(directory);
    }
}

std::ofstream BaseApp::CreateFile(const std::filesystem::path& file)
{
    std::string pathString = file.string();
    std::filesystem::path fileDirectory = file.parent_path();
    CreateDirectory(fileDirectory);

    std::ofstream File(pathString);
    return File;
}