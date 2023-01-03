#pragma once

template<typename T>
struct Setting
{
    Setting(T defaultSetting, std::string name) : setting(defaultSetting), newSetting(defaultSetting), settingName(name) {};
    bool newSettingValid() { return true; }
    bool tryChangeSetting()
    {
        if (newSettingValid())
        {
            insertNewSetting();
            showIncorrect = false;
            return true;
        } 
        showIncorrect = true;
        return false;
    }

    bool newSettingEqual() {
        return setting == newSetting;
    }

    std::string settingName;
    T getSetting() { return setting; };
    void editor() { ImGui::Text("editor not implemented for this template"); };

private:
    T newSetting;
    T setting;
    bool showIncorrect = false;
    void insertNewSetting();
};

class Settings
{
public:
    //TODO: should be changed to an array of param based setting
    static Setting<std::filesystem::path> gameDirectory;

    static void LoadSettings();
    static void SaveSettings();
};

template<typename T>
inline void Setting<T>::insertNewSetting()
{
    setting = newSetting;
}

template<>
inline bool Setting<std::filesystem::path>::newSettingValid()
{
    return std::filesystem::exists(newSetting);
}

template<>
inline void Setting<std::filesystem::path>::editor()
{
    ImGui::Text(settingName.c_str());
    if (showIncorrect)
    {
        ImGui::PushStyleColor(ImGuiCol_FrameBg, { (float)134 / 255,(float)34 / 255,(float)34 / 255,(float)138 / 255 });
    }
    std::string file = newSetting.string();
    ImGui::InputText(("##" + settingName).c_str(), &file);
    if (showIncorrect)
    {
        ImGui::PopStyleColor();
    }

    newSetting = file;
}

template<>
inline bool Setting<std::filesystem::path>::newSettingEqual()
{
    return setting.compare(newSetting) == 0;
}


