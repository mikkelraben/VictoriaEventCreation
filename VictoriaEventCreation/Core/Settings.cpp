#include "pch.h"
#include "Settings.h"
#include "yaml-cpp/yaml.h"
#include "FilesysHelper.h"
#include "../Sound/BankLoad.h"

//Setting Callback
void setNewVolume();


Setting<std::filesystem::path> Settings::gameDirectory("", "Game Directory", std::function<void(void)>());
Setting<float> Settings::volume(1.0f, "Volume", setNewVolume);

void setNewVolume()
{
    Sound::SoundSystem::setVolume(Settings::volume.getSetting());
}

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
        gameDirectory = Setting(std::filesystem::path(nodes[gameDirectory.settingName].as<std::string>()), gameDirectory.settingName,gameDirectory.settingChanged);
        volume = Setting(nodes[volume.settingName].as<float>(), volume.settingName,volume.settingChanged);
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
        out << YAML::Key << volume.settingName << YAML::Value << volume.getSetting();

        out << YAML::EndMap;

        file << out.c_str();
        file.close();
    }
}
