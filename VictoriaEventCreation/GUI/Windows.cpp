#include "pch.h"
#include "../Core/pch.h"
#include "Windows.h"
#include "../Sound/BankLoad.h"
#include "../Core/imgui/imgui_internal.h"
#include "Widgets.h"
#include "ImGuiHelper.h"
#include "../Scripting Objects/Trigger.h"



void TriggerDebugInfo(Trigger& trigger)
{
    ImGui::Text(trigger.name.c_str());
    ImGui::Text(trigger.description.c_str());
    if (trigger.interfaceTrigger)
    {
        ImGui::Text("Interface Trigger");
    }
    if (trigger.node)
    {
        ImGui::Text(("scope_id: " + std::to_string(trigger.node->scopeInput.id.Get())).c_str());
        ImGui::Text(("node_id: " + std::to_string(trigger.node->id.Get())).c_str());
    }

    for (auto& parameter : trigger.parameters)
    {
        if (parameter.param)
        {
            ImGui::Text(parameter.param->name.c_str());
        }
        if (parameter.comparable)
        {
            ImGui::SameLine();
            ImGui::Text(" | Comparable");
        }
        else
        {
            ImGui::SameLine();
            ImGui::Text(" | Not Comparable");
        }
        if (auto pin = dynamic_cast<Param<Pin>*>(parameter.param))
        {
            ImGui::Text(("pin_id: " + std::to_string(pin->variable.id.Get())).c_str());  
        }

    }
    std::string scopes = "Scopes: ";

    for (auto& scope : Scripting::scopes)
    {
        if (trigger.scopes & scope.key)
        {
            scopes = scopes + scope.name + ", ";
        }
    }
    scopes.pop_back();
    scopes.pop_back();
    ImGui::Text(scopes.c_str());

    ImGui::Text("Connections: ");

    for (auto& connection : trigger.children)
    {
        ImGui::Text(("start id: " + std::to_string(connection.link.startId.Get())).c_str());
        ImGui::Text(("end id: " + std::to_string(connection.link.endId.Get())).c_str());
        ImGui::Text(("link id: " + std::to_string(connection.link.id.Get())).c_str());
        ImGui::Text(("other node id: " + std::to_string(connection.otherNode.id.Get())).c_str());
    }

}


void EventTool::Run()
{
    static std::vector<std::string> comparors = { "<", "<=", "=", "!=" , ">" , ">=" };

    ImGui::BeginTabBar("EventTool");
    if (ImGui::BeginTabItem("Preview"))
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
        ImGui::EndTabItem();
    }
    if (ImGui::BeginTabItem("Behaviour"))
    {
        // Node Editor Widget
        ed::SetCurrentEditor(editor_Context);
        
        auto size = ImGui::GetContentRegionAvail();
        size = size + ImVec2(0, -21);
        ed::Begin("Behaviour", size);
        bool tooltip = false;
        bool pinTooltip = false;
        
        //For each node
        for (auto& node : object.behaviourNodes)
        {
            ed::BeginNode(node->node->id);
            if (node->scopes)
            {
                node->node->scopeInput.DrawPin();
                pinTooltip |= ImGui::IsItemHovered();
                if (ImGui::IsItemHovered())
                {
                    hoveredPin = &node->node->scopeInput;
                }
                ImGui::SameLine();
            }
            ImGui::Text(node->name.c_str());
            tooltip |= ImGui::IsItemHovered();
            if (ImGui::IsItemHovered())
            {
                contextNodeId = node->node->id;
            }
            auto& nodeContent = ImGui::GetCurrentWindow()->ContentRegionRect;
            //ImGui::Text(std::to_string(nodeContent.GetWidth()).c_str());
            //ImGui::Text(std::to_string(ed::GetNodeSize(node->node->id).x).c_str());
            //ImGui::Text(std::to_string(ImGui::GetCurrentWindow()->DC.CursorPos.x).c_str());

            nodeContent = { nodeContent.Min, ImVec2{ImGui::GetCurrentWindow()->DC.CursorPos.x + 100,400} };
            if (node->parameters.size() == 1)
            {
                ImGui::SameLine();
            }

            //For each parameter in each node
            for (auto& parameter : node->parameters)
            {
                ImGui::Text(parameter.param->name.c_str());
                if (parameter.comparable)
                {
                    auto clip = ImGui::GetCurrentWindow()->DrawList->CmdBuffer.back().ClipRect;
                    ImGui::PushID(&parameter);
                    ImGui::SameLine();
                    if (VecGui::Button(comparors[parameter.comparer].c_str(),true,{32,32}))
                    {
                        parameter.comparer = (Comparors)((int)(parameter.comparer + 1) % 6);
                    }

                    //if (VecGui::BeginCombo("##Comparors", comparors[parameter.comparer].c_str(), 0))
                    //{
                    //    for (size_t i = 0; i < comparors.size(); i++)
                    //    {
                    //        bool isSelected = parameter.comparer == i;
                    //        if (VecGui::Button(comparors[i].c_str(), true, { -1,32 }))
                    //        {
                    //            parameter.comparer = (Comparors)i;
                    //            ImGui::CloseCurrentPopup();
                    //        }
                    //
                    //        if (isSelected)
                    //        {
                    //            ImGui::SetItemDefaultFocus();
                    //        }
                    //    }
                    //    ImGui::EndCombo();
                    //    ed::Resume();
                    //}
                    ImGui::PopID();
                    ImGui::SameLine();
                }
                ImGui::SameLine();
                if (Param<Pin>* pin = dynamic_cast<Param<Pin>*>(parameter.param))
                {
                    if (pin->variable.scopes)
                    {
                        parameter.param->EditableField();
                        pinTooltip |= ImGui::IsItemHovered();
                        if (ImGui::IsItemHovered())
                        {
                            hoveredPin = &pin->variable;
                        }
                    }
                }
                else
                {
                    ImGui::PushItemWidth(200);
                    parameter.param->EditableField();
                    ImGui::PopItemWidth();
                }




            }
            ed::EndNode();
        }

        for (auto& node : object.behaviourNodes)
        {
            for (auto& child : node->children)
            {
                child.drawLink();
            }
        }

        auto popupPosition = ImGui::GetMousePos();

        ed::Suspend();

        bool justOpened = false;
        if (ed::ShowBackgroundContextMenu())
        {
            ImGui::OpenPopup("Create New Node");

            justOpened = true;
        }

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(12, 12));
        ImGui::SetNextWindowSizeConstraints({ 32,90 }, { -1,512 });
        if (ImGui::BeginPopup("Create New Node"))
        {
            if (justOpened)
            {
                ImGui::SetKeyboardFocusHere();
            }
            if (auto scriptingObject = Scripting::selectScriptingObject(popupPosition.x,popupPosition.y))
            {
                object.behaviourNodes.push_back(scriptingObject);
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }

        if (tooltip && contextNodeId.Get())
        {
            ImGui::BeginTooltip();
            ScriptingObject* selectedObject = nullptr;
            for (auto& node : object.behaviourNodes)
            {
                if (node->node->id == contextNodeId)
                {
                    selectedObject = node;
                    break;
                }
            }
            if (selectedObject)
            {
                ImGui::PushTextWrapPos(512);
                ImGui::Text(("Description: " + selectedObject->description).c_str());
                ImGui::PopTextWrapPos();
            }
            ImGui::Text("Active Scopes: ");
            for (auto& scope : Scripting::scopes)
            {
                if (selectedObject->activeScope & scope.key)
                {
                    ImGui::Text(scope.name.c_str());
                }
            }



            ImGui::EndTooltip();
        }

        if (hoveredPin && pinTooltip)
        {
            ImGui::BeginTooltip();
            
            ImGui::Text("Possible Scopes");
            for (auto& scope : Scripting::scopes)
            {
                if (hoveredPin->scopes & scope.key)
                {
                    ImGui::Text(scope.name.c_str());
                }
            }

            ImGui::EndTooltip();
        }

        ImGui::PopStyleVar();
        ed::Resume();

        //if new editor wants a link
        if (ed::BeginCreate())
        {
            ed::PinId inputPinId, outputPinId;
            if (ed::QueryNewLink(&inputPinId, &outputPinId))
            {
                createLink(inputPinId, outputPinId);
            }
        }
        ed::EndCreate();


        ed::End();
        ed::SetCurrentEditor(nullptr);

        ImGui::EndTabItem();
    }
#ifdef _DEBUG
    if (ImGui::BeginTabItem("Debug"))
    {

        static bool showTriggers = false;
        VecGui::CheckBox("Show Triggers", showTriggers);
        if (showTriggers)
        {
            ImGui::BeginChild("Triggers");
            static std::string filter;
            static std::vector<Trigger> allTriggers;
            if (allTriggers.empty())
            {
                for (auto& trigger : Scripting::triggers)
                {
                    allTriggers.emplace_back(trigger);
                }
            }
            
            ImGui::InputText("Filter", &filter);
            
            for (auto& trigger : allTriggers)
            {
                if (trigger.name.find(filter) != std::string::npos)
                {
                    TriggerDebugInfo(trigger);
                    ImGui::Separator();
                }
            }
            ImGui::EndChild();
        }

        static bool showScopes = false;
        VecGui::CheckBox("Show Scopes", showScopes);
        if (showScopes)
        {
            ImGui::BeginChild("Scopes");
            for (auto& scope : Scripting::scopes)
            {
                ImGui::Text(scope.name.c_str());
                ImGui::Text(std::to_string(scope.key).c_str());
                ImGui::Separator();
            }
            ImGui::EndChild();
        }

        static bool showObjectChildren = false;
        VecGui::CheckBox("Show Children", showObjectChildren);
        if (showObjectChildren)
        {
            ImGui::BeginChild("Children");
            for (auto& trigger : object.behaviourNodes)
            {
                Trigger* convertedTrigger = dynamic_cast<Trigger*>(trigger);
                if (convertedTrigger)
                {
                    TriggerDebugInfo(*convertedTrigger);
                }
                ImGui::Separator();
            }
            ImGui::EndChild();
        }

        static bool showEnums = false;
        VecGui::CheckBox("Show Enums", showEnums);
        if (showEnums)
        {
            ImGui::BeginChild("Enums");
            for (auto& enums : Scripting::enums)
            {
                ImGui::Text(enums.name.c_str());
                ImGui::Indent();
                for (auto option : enums.options)
                {

                    ImGui::Text(option.c_str());
                }
                ImGui::Unindent();
                ImGui::Separator();
            }
            ImGui::EndChild();
        }

        static bool showTypes = false;
        VecGui::CheckBox("Show Types", showTypes);
        if (showTypes)
        {
            ImGui::BeginChild("Types");
            for (auto& type : Scripting::types)
            {
                ImGui::Text(type.name.c_str());
                ImGui::Indent();
                for (auto option : type.options)
                {

                    ImGui::Text(option.c_str());
                }
                ImGui::Unindent();
                ImGui::Separator();
            }
            ImGui::EndChild();
        }

        static bool showTargets = false;
        VecGui::CheckBox("Show Targets", showTargets);
        if (showTargets)
        {
            ImGui::BeginChild("Targets");
            for (auto& type : Scripting::targets)
            {
                ImGui::Text(type.typeName.c_str());
                
                std::string scopes = "Input Scopes: ";
                for (auto& scope : Scripting::scopes)
                {
                    if (type.inputScopes & scope.key)
                    {
                        scopes = scopes + scope.name + ", ";
                    }
                }
                scopes.pop_back();
                scopes.pop_back();
                ImGui::Text(scopes.c_str());

                scopes = "Output Scopes: ";
                for (auto& scope : Scripting::scopes)
                {
                    if (type.outputScopes & scope.key)
                    {
                        scopes = scopes + scope.name + ", ";
                    }
                }
                scopes.pop_back();
                scopes.pop_back();
                ImGui::Text(scopes.c_str());

                if (type.globalLink)
                {
                    ImGui::Text("Global linkable");
                }
                if (type.requiresData)
                {
                    ImGui::Text("Requires Data");
                }

                ImGui::Separator();
            }
            ImGui::EndChild();
        }



        ImGui::EndTabItem();
    }
#endif // _DEBUG

    ImGui::EndTabBar();

}

void EventTool::createLink(ax::NodeEditor::PinId& inputPinId, ax::NodeEditor::PinId& outputPinId)
{

    if (!inputPinId.Invalid && !outputPinId.Invalid) // both are valid, let's accept link
    {
        if (inputPinId == outputPinId)
        {
            ed::RejectNewItem(ImColor(255, 0, 0), 2.0f);
            return;
        }

        Pin* input = nullptr;
        Pin* output = nullptr;
        ScriptingObject* inputNode = nullptr;
        ScriptingObject* outputNode = nullptr;
        findPin(inputPinId, input, inputNode);
        findPin(outputPinId, output, outputNode);

        if (!(input && output))
        {
            return;
        }

        if (input->pinKind == output->pinKind)
        {
            ed::RejectNewItem(ImColor(255, 0, 0), 2.0f);
            return;
        }

        if (!(input->scopes & output->scopes))
        {
            static scopeKey Scope;
            for (auto& scope : Scripting::scopes)
            {
                if (scope.name == "none")
                {
                    Scope = scope.key;
                    break;
                }
            }
            if (!((input->scopes & Scope) || (output->scopes & Scope)))
            {
                ed::RejectNewItem(ImColor(255, 0, 0), 2.0f);
                return;
            }
        }

        // ed::AcceptNewItem() return true when user release mouse button.
        if (ed::AcceptNewItem())
        {
            if (input && output && outputNode && inputNode)
            {
                if (input->pinKind == ed::PinKind::Output)
                {
                    inputNode->addChild(*input,*output,*outputNode);
                }
                else
                {
                    outputNode->addChild(*output, *input, *inputNode);
                }
            }
        }
    }
}

void EventTool::findPin(ax::NodeEditor::PinId& inputPinId, Pin*& input, ScriptingObject*& inputNode)
{
    for (auto& node : object.behaviourNodes)
    {
        if (inputPinId == node->node->scopeInput.id)
        {
            input = &node->node->scopeInput;
            inputNode = node;
        }
    }

    if (!input)
    {
        for (auto& node : object.behaviourNodes)
        {
            for (auto& parameter : node->parameters)
            {
                if (auto pin = dynamic_cast<Param<Pin>*>(parameter.param))
                {
                    if (inputPinId == pin->variable.id)
                    {
                        input = &pin->variable;
                        inputNode = node;
                    }
                }
            }
        }

    }
}


void EventTool::Properties()
{
    for (auto& param : object.children)
    {
        ImGui::Text(param->name.c_str());
        param->EditableField();
    }
}

void EventTool::Opened()
{
    ed::Config config;
    editor_Context = ed::CreateEditor(&config);
    
    ed::SetCurrentEditor(editor_Context);

    //DEV CODE
    Trigger* node = nullptr;
    for (auto& trigger : Scripting::triggers)
    {
        if (trigger.name == "commander_pm_usage")
        {
            node = new Trigger(trigger);
        }
    }

    if (node)
    {
        node->node = new NodeEditorNode({ 200,100 },node->scopes,PinShapes::circle);
        object.behaviourNodes.push_back(node);
    }

    //Create Trigger node
    Trigger* mainNode = nullptr;
    mainNode = new Trigger();
    mainNode->name = "Trigger";
    mainNode->description = "When conditions are satisfied this triggers the event";
    mainNode->scopes = 0;
    mainNode->node = new NodeEditorNode({ 0,0 }, mainNode->scopes, PinShapes::circle);
    scope* Scope = nullptr;
    for (auto& scope : Scripting::scopes)
    {
        if (scope.name == "country")
        {
            Scope = &scope;
            break;
        }
    }  

    if (Scope)
    {
        Param<Pin>* mainOutput = new Param<Pin>{ {ed::PinKind::Output,Scope->key,PinShapes::circle},"Trigger" };

        mainNode->parameters.emplace_back(mainOutput, false, Comparors::equal);
        mainNode->activeScope = Scope->key;
        object.behaviourNodes.push_back(mainNode);
    }

    ed::SetCurrentEditor(nullptr);
}

void EventTool::Exit()
{
    ed::DestroyEditor(editor_Context);
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
