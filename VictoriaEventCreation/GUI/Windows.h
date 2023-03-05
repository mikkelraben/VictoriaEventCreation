#pragma once
#include "../Core/Application.h"
#include "../Sound/BankLoad.h"
#include "../Scripting Objects/Event.h"
#include "ImGui Node Editor/imgui_node_editor.h"

class EventTool : public BaseApp::Window
{
public:
    explicit EventTool(std::vector<Window*>& applicationWindows) : Window(applicationWindows)
    {
        name = "Event Tool";
        isWindowOpen = true;
        windowFlags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;
    }
    void Run() override;
    void createLink(ax::NodeEditor::PinId& inputPinId, ax::NodeEditor::PinId& outputPinId);
    void findPin(ax::NodeEditor::PinId& inputPinId, Pin*& input, ScriptingObject*& inputNode);
    void Properties() override;
    void Opened() override;
    void Exit() override;
private:
    Event object;
    ax::NodeEditor::EditorContext* editor_Context = nullptr;
    ed::NodeId contextNodeId = 0;
    Pin* hoveredPin = nullptr;
};

class Console : public BaseApp::Window
{
public:
    explicit Console(std::vector<Window*>& applicationWindows) : Window(applicationWindows)
    {
        name = "Console";
        isWindowOpen = true;
    }
    void Run() override;
};

class SettingsEditor : public BaseApp::Window
{
public:
    explicit SettingsEditor(std::vector<Window*>& applicationWindows) : Window(applicationWindows)
    {
        name = "Settings";
        isWindowOpen = true;
    }
    void Run() override;
};

class Properties : public BaseApp::Window
{
public:
    explicit Properties(std::vector<Window*>& applicationWindows) : Window(applicationWindows)
    {
        name = "Properties";
        isWindowOpen = true;
    }
    void Run() override;
};

