#include "pch.h"
#include "../Core/pch.h"
#include "Windows.h"
#include "../Sound/BankLoad.h"
#include "Widgets.h"

void EventTool::Run()
{

}

void EventTool::Properties()
{
    for (auto& param : object.children)
    {
        param->EditableField();
    }

    static int selected = 0;
    static std::string preview = "Pick A Sound";
    if (ImGui::BeginCombo("Sounds", preview.c_str(), 0))
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


        if (VecGui::Button("Play", { sizeX,sizeY }))
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
    Settings::volume.editor();

    if (VecGui::Button("Save Settings"))
    {

        bool allSettingsCorrect = true;
        allSettingsCorrect &= Settings::gameDirectory.tryChangeSetting();
        allSettingsCorrect &= Settings::volume.tryChangeSetting();

        if (allSettingsCorrect)
        {
            Settings::SaveSettings();
        }
    }

    unsavedChanges = false;
    unsavedChanges |= !Settings::gameDirectory.newSettingEqual();
    unsavedChanges |= !Settings::volume.newSettingEqual();
}

void Properties::Run()
{
    static int SelectedID = -1;

    //find the current selection
    for (auto& window : windows)
    {
        //check if the window is selected and not the Properties window
        if (window->isSelected && window->getId() != windowID)
        {
            SelectedID = window->getId();
        }
    }

    //display the selection
    for (auto& window : windows)
    {
        if (window->getId() == SelectedID)
        {
            window->Properties();
            return;
        }
    }
    ImGui::Text("This should not appear unless you are selecting a debug window");
}
