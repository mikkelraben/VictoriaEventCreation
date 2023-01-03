#pragma once
namespace BaseApp
{
    void CreateDirectory(const std::filesystem::path& directory);
    std::ofstream CreateFile(const std::filesystem::path& file);
}

