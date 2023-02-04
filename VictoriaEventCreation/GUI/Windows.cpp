#include "pch.h"
#include "../Core/pch.h"
#include "Windows.h"
#include "../Sound/BankLoad.h"
#include "../Core/imgui/imgui_internal.h"
#include "Widgets.h"
#include "ImGuiHelper.h"

void EventTool::Run()
{
    ImVec2 size = { 1190,808 };
    ImGui::BeginChild("Event Window", size, false, VictoriaEventWindow);
    auto param = dynamic_cast<Param<std::string>*>(BasicNode::findChildFromName(object.children, "Title"));
    if (param)
    {
        std::string title = param->variable;
        ImFont* headerFont = ImGui::GetIO().Fonts->Fonts[2];
        ImGui::PushFont(headerFont);
        auto cursor = ImGui::GetCursorPos();
        ImVec2 text_size = ImGui::CalcTextSize(title.c_str(), 0, true);
        ImGui::SetCursorPos(cursor + ImVec2{ size.x / 2 - text_size.x / 2, 4});

        ImGui::Text(title.c_str());
        ImGui::PopFont();
    }
    ImGui::EndChild();
}

void EventTool::Properties()
{
    for (auto& param : object.children)
    {
        param->EditableField();
    }

    static int selected = 0;
    static std::string preview = "Pick A Sound";
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
