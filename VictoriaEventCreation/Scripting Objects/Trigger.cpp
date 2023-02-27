#include "pch.h"
#include "Trigger.h"
#include <execution>

std::vector<PossibleTrigger> Scripting::triggers = {};
std::vector<scope> Scripting::scopes = {};
std::vector<ScriptingEnum> Scripting::enums = {};
std::vector<ScriptingType> Scripting::types = {};
std::vector<scope> Scripting::utilizedScopes = {};



enum class TriggersState
{
    none,
    title,
    read
};

Trigger* Scripting::selectTrigger()
{
    static std::string filter;
    ImGui::InputText("Filter", &filter);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, {0,0});
    if (filter.empty())
    {
        for (auto& scope : utilizedScopes)
        {
            if (ImGui::TreeNode(scope.name.c_str()))
            {
                for (auto& scriptingTrigger : triggers)
                {
                    if (scriptingTrigger.scopes & scope.key && !scriptingTrigger.interfaceTrigger)
                    {

                        if (VecGui::Button(scriptingTrigger.name, true,{ -1,32 }))
                        {
                            ImGui::TreePop();
                            ImGui::PopStyleVar();
                            return new Trigger(scriptingTrigger);
                        }
                    }
                }
                ImGui::TreePop();
            }
        }
    }
    else
    {
        for (auto& scriptingTrigger : triggers)
        {
            if (scriptingTrigger.name.find(filter) != std::string::npos && !scriptingTrigger.interfaceTrigger)
            {
                if (VecGui::Button(scriptingTrigger.name, true,{ -1,32 }))
                {
                    ImGui::PopStyleVar();
                    return new Trigger(scriptingTrigger);
                }
            }
        }

    }

    ImGui::PopStyleVar();

    return nullptr;
}

void Scripting::catalogueAllTriggers()
{
    constexpr auto max_size = std::numeric_limits<std::streamsize>::max();
    std::ifstream file("Scripting Docs/triggers.log");
    if (file.is_open())
    {
        file.seekg(48);

        PossibleTrigger trigger;
        TriggersState state = TriggersState::none;

        while (file.good())
        {
            std::string line;
            std::getline(file, line, '\n');
            if (line == "")
            {
                if (state == TriggersState::read)
                {
                    Scripting::triggers.push_back(trigger);
                    trigger = PossibleTrigger{};
                }
                continue;
            }

            if (line == "--------------------")
            {
                state = TriggersState::title;
                continue;
            }

            if (state == TriggersState::title)
            {
                auto titleLength = line.find(' ');
                if (titleLength == std::string::npos)
                {
                    RE_LogError("Could not find a space when it was expected");
                }
                auto title = line.substr(0, titleLength);
                trigger.name = title;

                auto description = line.substr(titleLength+3);
                trigger.description = description;



                state = TriggersState::read;
                continue;
            }

            if (line.starts_with("Supported Scopes: "))
            {
                auto scopes = line.substr(18);

                bool moreScopesAvailable = true;
                size_t offset = 0;
                while (moreScopesAvailable)
                {
                    std::string scope;
                    auto startOffset = offset;
                    offset = scopes.find_first_of(',', offset);
                    if (offset == std::string::npos)
                    {
                        moreScopesAvailable = false;
                        scope = scopes.substr(startOffset);
                    }
                    else
                    {
                        scope = scopes.substr(startOffset, offset - startOffset);
                        offset += 2;
                    }
                    for (auto& scopeItem : Scripting::scopes)
                    {
                        if (scopeItem.name == scope)
                        {
                            trigger.scopes |= scopeItem.key;
                        }
                    }
                    

                }
            }

            if (line.starts_with("An interface trigger"))
            {
                trigger.interfaceTrigger = true;
            }
        }

        file.close();
    }
    else
    {
        RE_LogError("Could not find \"Scripting Docs/triggers.log\"");
    }

    file = std::ifstream{ "Scripting Docs/cwtools-vic3-config-master/config/triggers.cwt" };
    if (file.is_open())
    {
        TriggersState state = TriggersState::none;
        PossibleTrigger* trigger = nullptr;
        while (file.good())
        {
            std::string line;
            std::getline(file, line, '\n');
            if (line == "")
            {
                continue;
            }

            if (line.starts_with('#'))
            {
                state = TriggersState::title;
                continue;
            }

            if (line.starts_with("alias[trigger:"))
            {
                auto Length = line.find_first_of(']');
                auto title = line.substr(14,Length-14);

                trigger = nullptr;
                for (auto& Existingtrigger : Scripting::triggers)
                {
                    if (Existingtrigger.name == title)
                    {
                        trigger = &Existingtrigger;
                    }
                }
                if (!trigger)
                {
                    state = TriggersState::none;
                    continue;
                }
                if (trigger->name == "")
                {
                    RE_LogWarning("Could not find trigger: " + title);
                }

                if (line.ends_with('{'))
                {
                    state = TriggersState::read;
                }
                else
                {
                    auto type = line.substr(Length+1);

                    trigger->paramInputs.push_back(type);
                }

                continue;
            }

            if (line.starts_with('}'))
            {
                state = TriggersState::none;
                continue;
            }

            if (state == TriggersState::read)
            {
                if (trigger)
                {
                    trigger->paramInputs.push_back(line);
                }
                continue;

            }
        }

        file.close();
    }
    else
    {
        RE_LogError("Could not find \"Scripting Docs/triggers.log\"");
    }

    //calculate utilizedScopes
    scopeKey usedScopes = 0;
    for (auto& trigger : triggers)
    {
        usedScopes |= trigger.scopes;
    }
    for (auto& scope : scopes)
    {
        if (scope.key & usedScopes)
        {
            utilizedScopes.push_back(scope);
        }
    }

    //find any empty triggers
    for (auto& trigger : triggers)
    {
        if (trigger.paramInputs.empty())
        {
            RE_LogWarning("This trigger has no parameters: " + trigger.name);
        }
    }

}

void Scripting::catalogueAllScopes()
{
    constexpr auto max_size = std::numeric_limits<std::streamsize>::max();

    std::ifstream file("Scripting Docs/event_scopes.log");
    if (file.is_open())
    {
        //ignore header
        file.ignore(max_size, '\n');
        unsigned long long i = 0;
        while (file.good())
        {
            std::string scopeName;
            std::getline(file, scopeName, '\n');
            if (scopeName.length() > 1)
            {
                scopeKey key = 1LL << i++;
                Scripting::scopes.push_back({ scopeName, key });
            }
        }
        file.close();
    }
    else
    {
        RE_LogError("Could not find \"Scripting Docs/event_scopes.log\"");
    }


}

void Scripting::catalogueAllEnums()
{
    std::ifstream file = std::ifstream{ "Scripting Docs/cwtools-vic3-config-master/config/enums.cwt" };
    if (file.is_open())
    {
        ScriptingEnum tempEnum;
        while (file.good())
        {
            std::string line;
            std::getline(file, line, '\n');

            if (line.starts_with("    enum["))
            {
                tempEnum = {};

                auto name = line.substr(9, line.find_first_of(']')-9);
                tempEnum.name = name;
                continue;
            }

            if (line == "    }")
            {
                enums.push_back(tempEnum);
                continue;
            }

            if (line.starts_with("        "))
            {
                tempEnum.options.push_back(line.substr(8));
                continue;
            }
        }
        file.close();
    }
}

void Scripting::catalogueAllTypes()
{
    struct TypeNameAndPath
    {
        std::string typeName;
        std::string relativePath;
    };

    std::filesystem::path currentPath = "Scripting Docs/cwtools-vic3-config-master/config/common/";

    std::vector<TypeNameAndPath> initialTypesInfo;

    for (auto& dir_entry : std::filesystem::directory_iterator{ currentPath })
    {
        std::ifstream file = std::ifstream(dir_entry.path().string());
        if (file.is_open())
        {
            while (file.good())
            {
                std::string line;
                std::getline(file, line, '\n');
                if (line.starts_with("    type["))
                {
                    initialTypesInfo.emplace_back(line.substr(9,line.find_first_of(']') - 9),"");
                    continue;
                }

                if (line.starts_with("        path = \""))
                {
                    initialTypesInfo.back().relativePath = line.substr(16, line.find_first_of('\"',17) - 16);
                    continue;
                }
            }

            file.close();
        }

    }

    types.resize(initialTypesInfo.size());
    std::atomic<int> iterator = 0;
    std::for_each(std::execution::par, initialTypesInfo.begin(), initialTypesInfo.end(), [&](TypeNameAndPath type)
    {
        int i = iterator++;
        types[i].name = type.typeName;
        std::filesystem::path absolutePath = Settings::gameDirectory.getSetting() / type.relativePath;
        //RE_LogMessage("Type: " + type.typeName + " Path: " + absolutePath.generic_string());
        if (std::filesystem::exists(absolutePath))
        {
            for (auto& dir_entry : std::filesystem::directory_iterator{ absolutePath })
            {
                if (dir_entry.path().extension().string() == ".info")
                {
                    continue;
                }

                std::ifstream file = std::ifstream(dir_entry.path().string());

                if (file.is_open())
                {
                    //skip BOM
                    if (file.peek() == 0xEF)
                    {
                        file.seekg(3);
                    }

                    while (file.good())
                    {
                        std::string line;
                        std::getline(file, line, '\n');

                        if (line == "")
                        {
                            continue;
                        }
                        if (line.starts_with('#'))
                        {
                            continue;
                        }
                        if (line.starts_with(' '))
                        {
                            continue;
                        }
                        if (line.starts_with('\t'))
                        {
                            continue;
                        }
                        if (line.starts_with('}'))
                        {
                            continue;
                        }

                        types[i].options.emplace_back(line.substr(0, line.find_first_of(' ')));
                    }


                    file.close();
                }
                else
                {
                    RE_LogError("Could not open file: " + dir_entry.path().string());
                }
            }
        }
        else
        {
            RE_LogError("Path doesn't exist");
        }
    });

}

std::string Scripting::beautifyName(std::string input)
{
    std::string formatted = input.data();
    if (!formatted.empty())
    {
        formatted[0] = std::toupper(formatted[0]);
        bool underScoresExist = true;
        size_t offset = 0;
        while (underScoresExist)
        {
            auto underscorePosition = formatted.find_first_of('_', offset);
            if (underscorePosition == std::string::npos)
            {
                break;
            }
            formatted[underscorePosition] = ' ';
            if (formatted.size() > underscorePosition + 1)
            {
                formatted[underscorePosition + 1] = std::toupper(formatted[underscorePosition + 1]);
            }
            offset = underscorePosition + 1;
        }
        return formatted;
    }

    return std::string();
}

void Scripting::addParam(std::string_view input, std::vector<ScriptingParam>& list)
{
    ScriptingParam param;
    std::string name;

    auto spaces = input.find_first_not_of(' ');
    auto noSpaces = input.substr(spaces);

    if (noSpaces.find_first_of('#') != std::string::npos)
    {
        return;
    }

    auto beforeEquals = noSpaces.substr(0, noSpaces.find_first_of('='));
    if (!beforeEquals.empty())
    {
        name = beforeEquals.substr(0, beforeEquals.find_first_of(' '));
    }


    auto equals = noSpaces.find_last_of('=') - noSpaces.find_first_of('=');
    if (equals == 1)
    {
        param.comparable = true;
    }

    std::string_view afterEquals = "";
    if (noSpaces.find_last_of('=') != std::string::npos)
    {
        afterEquals = noSpaces.substr(noSpaces.find_last_of('=') + 1);
        spaces = afterEquals.find_first_not_of(' ');
        afterEquals = afterEquals.substr(spaces);
    }

    if (name.empty())
    {
        name = afterEquals;
    }

    param.param = getParam(afterEquals);
    if (param.param->name.empty() || param.param->name == "@@node")
    {
        param.param->name = name;
    }

    auto var = name.find("replace_me");
    if (var == std::string::npos)
    {
        list.push_back(param);
    }
}

int Scripting::countScope(scopeKey scope)
{
#ifdef _MSC_VER
    return __popcnt64(scope);
#else
#error "countScope not supported on this compiler yet"
#endif // _MSC_VER
    
}

BasicNode* Scripting::getParam(std::string_view input)
{
    if (input == "bool")
    {
        return new Param<bool>(false, "@@node");
    }
    if (input == "value_field")
    {
        return new Param<float>(0, "");
    }
    if (input == "int")
    {
        return new Param<int>(0, "");
    }
    if (input.starts_with("enum["))
    {
        auto enumName = input.substr(5,input.find_first_of(']')-5);
        for (auto& scriptEnum : enums)
        {
            if (scriptEnum.name == enumName)
            {
                return new Param<ScriptingEnum>(scriptEnum, "");
            }
        }
    }
    if (input.starts_with("scope["))
    {
        auto scopeName = input.substr(6, input.find_first_of(']') - 6);
        for (auto& scriptScope : scopes)
        {
            if (scriptScope.name == scopeName)
            {
                return new Param<Pin>({ed::PinKind::Output,scriptScope.key}, "");
            }
        }
    }

    if (input.starts_with("<"))
    {
        auto typeName = input.substr(1, input.find_first_of('>') - 1);
        for (auto& type : types)
        {
            if (type.name == typeName)
            {
                return new Param<ScriptingType>(type, "");
            }
        }
    }



    RE_LogWarning(std::string("Could not find suitable param for given input: ") + input.data());

    return new Param<NoBehaviour>({input}, "");
}


void Param<NoBehaviour>::EditableField()
{
    ImGui::Text(("No Behaviour Yet! Recieved: " + variable.RightHandSide).c_str());
}

YAML::Node Param<NoBehaviour>::Serialize()
{
    YAML::Node node;
    node["type"] = "param";
    node["name"] = name;
    node["variable"] = "undefined";
    node["variableType"] = "undefined";

    return node;
}

Trigger::Trigger(PossibleTrigger& trigger)
{
    name = trigger.name;
    description = trigger.description;
    for (auto& input : trigger.paramInputs)
    {
        Scripting::addParam(input, parametersType);
    }
    scopes = trigger.scopes;
    interfaceTrigger = trigger.interfaceTrigger;
}

YAML::Node Trigger::Serialize()
{
    return YAML::Node();
}

void Trigger::Deserialize(const YAML::Node& node)
{

}

void Pin::DrawPin()
{
    if (scopes)
    {
        ImGui::PushID(this);
        if (pinKind == ed::PinKind::Input)
        {
            ed::BeginPin(id, pinKind);
            ImGui::Text("->");
            ed::EndPin();
        }
        ImGui::SameLine();
        for (auto& scriptScope : Scripting::scopes)
        {
            if (scopes & scriptScope.key)
            {
                ImGui::Text(scriptScope.name.c_str());
                ImGui::SameLine();
            }
        }
        if (pinKind == ed::PinKind::Output)
        {
            ed::BeginPin(id, pinKind);
            ImGui::Text("->");
            ed::EndPin();
        }
        ImGui::PopID();
        
    }
}

void NodeConnection::drawLink()
{
    ed::Link(link.id, link.startId, link.endId);
}
