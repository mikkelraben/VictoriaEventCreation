#include "pch.h"
#include "Settings.h"
#include "yaml-cpp/yaml.h"
#include "FilesysHelper.h"

Setting<std::filesystem::path> Settings::gameDirectory("","Game Directory");

void Settings::LoadSettings()
{
    auto fileLocation = std::filesystem::current_path() / "settings.ini";
    YAML::Node nodes;
    try
    {
        nodes = YAML::LoadFile(fileLocation.string());
    }
    catch (const YAML::Exception& e)
    {
        RE_LogError("No settings available");
        RE_LogError(e.msg);
        return;
    }

    try
    {
        gameDirectory = Setting(std::filesystem::path(nodes[gameDirectory.settingName].as<std::string>()), gameDirectory.settingName);
    }
    catch (const YAML::Exception& e)
    {
        RE_LogError(e.msg);
    }

}

void Settings::SaveSettings()
{
    auto fileLocation = std::filesystem::current_path() / "settings.ini";
    auto file = BaseApp::CreateFile(fileLocation);
    if (file)
    {
        YAML::Emitter out;
        out << YAML::BeginMap;
        out << YAML::Key << gameDirectory.settingName << YAML::Value << gameDirectory.getSetting().string();

        out << YAML::EndMap;

        file << out.c_str();
        file.close();
    }
}
