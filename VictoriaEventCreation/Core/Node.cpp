#include "pch.h"
#include "Node.h"

void Node::EditableField()
{
    ImGui::Text("Hello There");
}

YAML::Node Node::Serialize()
{
    YAML::Node node;
    node["type"] = "node";
    node["name"] = name;
    node["possibleScope"] = possibleScope;
    node["formalName"] = formalName;
    YAML::Node childrenNode;
    for (auto& child : children)
    {
        childrenNode.push_back(child->Serialize());
    }
    node["children"] = childrenNode;

    return node;
}

void Node::Deserialize(const YAML::Node& node)
{
    RE_ASSERT(node["type"].as<std::string>() == "node");

    name = node["name"].as<std::string>();
    int newScope = 0;

    formalName = node["formalName"].as<std::string>();
    YAML::Node childrenNode = node["children"];
    for (auto child : childrenNode)
    {
        BasicNode* newChild;
        if (child["type"].as<std::string>() == "node")
        {
            newChild = new Node("", "");
        }
        else if (child["type"].as<std::string>() == "param")
        {
            if (child["variableType"].as<std::string>() == "int")
            {
                newChild = new Param<int>(0, "");
            }
            else
            {
                RE_LogError("Could not deduce variable type");
                return;
            }
        }
        else
        {
            RE_LogError("Child node not node or param");
            return;
        }
        newChild->Deserialize(child);
        children.push_back(newChild);
    }
}

template<>
void Param<int>::EditableField()
{
    ImGui::Text(name.c_str());
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
void Param<std::string>::EditableField()
{
    ImGui::Text(name.c_str());
    ImGui::SameLine();
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
