#include "pch.h"
#include "Node.h"
#include "../Sound/BankLoad.h"
#include "../Scripting Objects/Trigger.h"

BasicNode* BasicNode::findChildFromName(std::vector<BasicNode*>& children, std::string_view name)
{
    for (auto& child : children)
    {
        if (child->name == name)
        {
            return child;
        }
    }
    return nullptr;
}

template<>
void Param<int>::EditableField()
{
    ImGui::SameLine();
    ImGui::PushItemWidth(-4);
    ImGui::PushID(&name);
    ImGui::InputInt(("##" + name).c_str(), &variable);
    ImGui::PopID();
    ImGui::PopItemWidth();
}

template<>
YAML::Node Param<int>::Serialize()
{
    YAML::Node node;
    node["type"] = "param";
    node["name"] = name;
    node["variable"] = variable;
    node["variableType"] = "int";

    return node;
}


template<>
void Param<float>::EditableField()
{
    ImGui::SameLine();
    ImGui::PushItemWidth(-4);
    ImGui::PushID(&name);
    ImGui::InputFloat(("##" + name).c_str(), &variable);
    ImGui::PopID();
    ImGui::PopItemWidth();
}

template<>
YAML::Node Param<float>::Serialize()
{
    YAML::Node node;
    node["type"] = "param";
    node["name"] = name;
    node["variable"] = variable;
    node["variableType"] = "float";

    return node;
}


template<>
void Param<bool>::EditableField()
{
    ImGui::SameLine();
    ImGui::PushItemWidth(-4);
    ImGui::PushID(&name);
    VecGui::CheckBox(name, variable);
    ImGui::PopID();
    ImGui::PopItemWidth();
}

template<>
YAML::Node Param<bool>::Serialize()
{
    YAML::Node node;
    node["type"] = "param";
    node["name"] = name;
    node["variable"] = variable;
    node["variableType"] = "bool";

    return node;
}


template<>
void Param<std::string>::EditableField()
{
    ImGui::PushItemWidth(-4);
    ImGui::PushID(&name);
    ImGui::InputText(("##" + name).c_str(), &variable);
    ImGui::PopID();
    ImGui::PopItemWidth();
}

template<>
YAML::Node Param<std::string>::Serialize()
{
    YAML::Node node;
    node["type"] = "param";
    node["name"] = name;
    node["variable"] = variable;
    node["variableType"] = "string";

    return node;
}


void Param<Sound::Event>::EditableField()
{
    ImGui::PushID(&name);
    if (VecGui::CheckBox("UI", ui))
    {
        if (event)
        {
            event = !event;
        }
    }
    ImGui::SameLine();

    if (VecGui::CheckBox("Event", event))
    {
        if (ui)
        {
            ui = !ui;
        }
    }

    std::string filter = "";
    if (ui)
    {
        filter = "event:/SFX/UI/Alerts/";
    }

    if (event)
    {
        filter = "event:/SFX/Events/";
    }

    ImGui::PushItemWidth(-4);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0,0 });
    if (VecGui::BeginCombo("##Sounds", preview.c_str(), 0))
    {
        for (size_t i = 0; i < soundSystem.events.size(); i++)
        {
            if (soundSystem.events[i].name.starts_with(filter))
            {
                bool isSelected = selected == i;
                if (VecGui::Button(soundSystem.events[i].name.c_str(), true, { -1,32 }))
                {
                    soundSystem.events[selected].Stop();
                    preview = soundSystem.events[i].name;
                    selected = i;
                    ImGui::CloseCurrentPopup();
                }

                if (isSelected)
                {
                    ImGui::SetItemDefaultFocus();
                }
            }
        }
        ImGui::EndCombo();
    }
    ImGui::PopStyleVar();
    if (!soundSystem.events.empty())
    {
        if (VecGui::Button("Play"))
        {
            soundSystem.events[selected].Play();
        }
        ImGui::SameLine();
        if (VecGui::RoundButton({ "stop" }, "close", { 47,47 }))
        {
            soundSystem.events[selected].Stop();
        }
    }

    ImGui::PopID();
    ImGui::PopItemWidth();
}

YAML::Node Param<Sound::Event>::Serialize()
{
    YAML::Node node;
    node["type"] = "param";
    node["name"] = name;
    node["variable"] = variable;
    node["variableType"] = "string";

    return node;
}

void Param<Sound::Event>::FindSelection()
{
    if (Sound::SoundSystem::events.size() > 0)
    {
        for (size_t i = 0; i < Sound::SoundSystem::events.size(); i++)
        {
            if (Sound::SoundSystem::events[i].name == variable.name)
            {
                selected = i;
                preview = Sound::SoundSystem::events[i].name;
                return;
            }
        }
    }
}


void Param<ScriptingEnum>::EditableField()
{
    ImGui::PushID(&name);
    ImGui::PushItemWidth(200);
    std::string preview;
    if (variable.selected < variable.options.size())
    {
        preview = variable.options[variable.selected];
    }
    else
    {
        preview = variable.options[0];
    }

    bool insideNode = ed::GetCurrentEditor();
    
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0,0 });

    if (VecGui::BeginCombo("##Enums", preview.c_str(), 0))
    {
        for (size_t i = 0; i < variable.options.size(); i++)
        {
            bool isSelected = variable.selected == i;
            if (VecGui::Button(variable.options[i].c_str(),true,{-1,32}))
            {
                variable.selected = i;
                ImGui::CloseCurrentPopup();
            }

            if (isSelected)
            {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
        if (insideNode)
        {
            ed::Resume();
        }
    }
    ImGui::PopStyleVar();


    ImGui::PopID();
    ImGui::PopItemWidth();
}

YAML::Node Param<ScriptingEnum>::Serialize()
{
    YAML::Node node;
    node["type"] = "param";
    node["name"] = name;
    node["variable"] = variable;
    node["variableType"] = "Enum";

    return node;
}

void Param<ScriptingType>::EditableField()
{
    ImGui::PushID(&name);
    ImGui::PushItemWidth(200);
    std::string preview;
    if (variable.selected < variable.options.size())
    {
        preview = variable.options[variable.selected];
    }
    else
    {
        preview = variable.options[0];
    }

    bool insideNode = ed::GetCurrentEditor();

    static std::string filter = "";
    
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(12, 12));
    ImGui::SetNextWindowSizeConstraints({ 32,90 }, { -1,512 });
    if (VecGui::BeginCombo("##Types", preview.c_str(), 0))
    {
        ImGui::InputText("Filter", &filter);
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0,0 });

        for (size_t i = 0; i < variable.options.size(); i++)
        {
            if (variable.options[i].find(filter) != std::string::npos)
            {
                bool isSelected = variable.selected == i;
                if (VecGui::Button(variable.options[i].c_str(), true, { -1,32 }))
                {
                    variable.selected = i;
                    ImGui::CloseCurrentPopup();
                }

                if (isSelected)
                {
                    ImGui::SetItemDefaultFocus();
                }
            }
        }
        ImGui::EndCombo();
        ImGui::PopStyleVar();
        if (insideNode)
        {
            ed::Resume();
        }
    }
    ImGui::PopStyleVar();



    ImGui::PopID();
    ImGui::PopItemWidth();
}

YAML::Node Param<ScriptingType>::Serialize()
{
    YAML::Node node;
    node["type"] = "param";
    node["name"] = name;
    node["variable"] = variable;
    node["variableType"] = "Type";

    return node;
}

void Param<Pin>::EditableField()
{
    variable.DrawPin();
}

YAML::Node Param<Pin>::Serialize()
{
    YAML::Node node;
    node["type"] = "param";
    node["name"] = name;
    node["variable"] = variable;
    node["variableType"] = "pin";

    return node;
}
