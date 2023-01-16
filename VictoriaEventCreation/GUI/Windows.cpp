#include "pch.h"
#include "../Core/pch.h"
#include "Windows.h"
#include "../Sound/BankLoad.h"
#include "Widgets.h"

void EventTool::Run()
{
    static int selected = 0;
    static std::string preview = "Pick A Sound";
    if (ImGui::BeginCombo("Sounds",preview.c_str(), 0))
    {
        for (size_t i = 0; i < soundSystem.events.size(); i++)
        {
            bool isSelected = selected == i;
            if (ImGui::Selectable(soundSystem.events[i].name.c_str()))
            {
                preview = soundSystem.events[i].name;
                selected = i;
            }

            if (isSelected)
            {
                ImGui::SetItemDefaultFocus();
                
            }
        }
        ImGui::EndCombo();
    }
    if (!soundSystem.events.empty())
    {
        static float sizeX = 0.0f;
        static float sizeY = 0.0f;

        VecGui::SliderFloat("Size X", sizeX, -1, 256);
        VecGui::SliderFloat("Size Y", sizeY, -1, 256);


        if(VecGui::Button("Play",{sizeX,sizeY}))
        {
            soundSystem.events[selected].Play();
        }

        if (VecGui::RoundButton("stop", "close", { 47,47 }))
        {
            soundSystem.events[selected].Stop();
        }


        static bool Value = false;

        VecGui::CheckBox("CheckBox", Value);
    }

}

void EventTool::Opened()
{
}

void Console::Run()
{
    ImGuiListClipper clipper;
    clipper.Begin((int)Log::logs.size());
    while (clipper.Step())
    {
        for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
        {
            ImGui::TextUnformatted((Log::logs[i].type + Log::logs[i].log).c_str());
        }
    }
}

void SettingsEditor::Run()
{
    Settings::gameDirectory.editor();

    if (VecGui::Button("Save Settings"))
    {
        if (Settings::gameDirectory.tryChangeSetting())
        {
            Settings::SaveSettings();
        }
    }

    unsavedChanges = !Settings::gameDirectory.newSettingEqual();
}
