#pragma once
#include "../Core/Node.h"
#include "ImGui Node Editor/imgui_node_editor.h"

namespace ed = ax::NodeEditor;

typedef unsigned long long scopeKey;

static int getId() { static int id = 1; return id++; };

struct scope
{
    std::string name;
    scopeKey key;
};

enum Comparors
{
    lessThan,
    lessThanOrEqual,
    equal,
    notEqual,
    largerThan,
    largerThanOrEqual
};

class ScriptingParam
{
public:
    BasicNode* param;
    bool comparable = false;
    Comparors comparer;
};

struct NoBehaviour
{
    NoBehaviour() {};
    NoBehaviour(std::string_view input) { RightHandSide = input.data(); };
    std::string RightHandSide;
};

struct ScriptingEnum
{
    std::string name;
    std::vector<std::string> options;
    size_t selected = (size_t) -1;
};

struct ScriptingType
{
    std::string name;
    std::vector<std::string> options;
    size_t selected = (size_t)-1;
};

enum class PinShapes
{
    square,
    circle,
};

struct Pin
{
    Pin() { Pin(ed::PinKind::Input, 0); };
    Pin(ed::PinKind type, scopeKey supportedScopes)
    {
        id = getId();
        pinKind = type;
        scopes = supportedScopes;
    }
    ed::PinId id;
    ed::PinKind pinKind;
    scopeKey scopes;

    void DrawPin();
};

struct Link
{
    Link(ed::PinId thisId, ed::PinId otherId)
    {
        id = getId();
        startId = thisId;
        endId = otherId;
    }
    ed::LinkId id;

    ed::PinId startId;
    ed::PinId endId;
};

class NodeEditorNode
{
public:
    NodeEditorNode(ImVec2 position, scopeKey scopes) : scopeInput(ed::PinKind::Input, scopes)
    {
        id = getId();
        x = position.x;
        y = position.y;
        ed::SetNodePosition(id, position);
    };

    ed::NodeId id;
    Pin scopeInput;

    //position
    float x = 0, y = 0;
};

struct NodeConnection
{
    NodeConnection(Pin& thisPin, Pin& endPin, NodeEditorNode& endNode) : link(thisPin.id, endPin.id), otherNode(endNode)
    {

    };
    NodeConnection(NodeEditorNode& thisNode, NodeEditorNode& endNode) : link(thisNode.scopeInput.id,endNode.scopeInput.id), otherNode(endNode)
    {

    };
    Link link;
    NodeEditorNode& otherNode;
    void drawLink();
};

struct PossibleTrigger
{
    std::string name;
    std::string description;
    std::vector<std::string> paramInputs;
    scopeKey scopes = 0;
    bool interfaceTrigger = false;
};

struct Trigger : public BasicNode
{
    Trigger() = default;
    Trigger(PossibleTrigger& trigger);
    YAML::Node Serialize() override;
    void Deserialize(const YAML::Node& node);

    NodeEditorNode* node = nullptr;

    std::string description;

    // other triggers
    std::vector<NodeConnection> children;

    //parameters inside node
    std::vector<ScriptingParam> parametersType;
    scopeKey scopes = 0;
    bool interfaceTrigger = false;
};


struct Scripting
{
    static void InitScripting() { RE_LogMessage("Initializing Scripting"); catalogueAllScopes(); catalogueAllEnums(); catalogueAllTypes(); catalogueAllTriggers();  };
    static std::vector<PossibleTrigger> triggers;
    static std::vector<scope> scopes;
    static std::vector<ScriptingEnum> enums;
    static std::vector<ScriptingType> types;
    //subset of scopes inwhich a trigger exists
    static std::vector<scope> utilizedScopes;

    static Trigger* selectTrigger();
    static void addParam(std::string_view input, std::vector<ScriptingParam>& list);
    int countScope(scopeKey scope);
    
private:
    static void catalogueAllTriggers();
    static void catalogueAllScopes();
    static void catalogueAllEnums();
    static void catalogueAllTypes();
    static std::string beautifyName(std::string input);

    static BasicNode* getParam(std::string_view input);
};

namespace YAML
{
    template<>
    struct convert<NoBehaviour>
    {
        static Node encode(const NoBehaviour& rhs)
        {
            Node node;
            return node;
        }

        static bool decode(const Node& node, NoBehaviour& rhs)
        {
            return true;
        }
    };


    template<>
    struct convert<ScriptingEnum>
    {
        static Node encode(const ScriptingEnum& rhs)
        {
            Node node;

            node.push_back(rhs.name);
            node.push_back(rhs.selected);
            return node;
        }

        static bool decode(const Node& node, ScriptingEnum& rhs)
        {
            if (!node.IsSequence() && node.size() == 2)
                return false;
            auto enumName = node[0].as<std::string>();
            for (auto& scriptEnum : Scripting::enums)
            {
                if (scriptEnum.name == enumName)
                {
                    rhs = scriptEnum;
                }
            }
            rhs.selected = node[1].as<std::size_t>();

            return true;
        }
    };

    template<>
    struct convert<ScriptingType>
    {
        static Node encode(const ScriptingType& rhs)
        {
            Node node;

            node.push_back(rhs.name);
            node.push_back(rhs.selected);
            return node;
        }

        static bool decode(const Node& node, ScriptingType& rhs)
        {
            if (!node.IsSequence() && node.size() == 2)
                return false;
            auto enumName = node[0].as<std::string>();
            for (auto& scriptEnum : Scripting::types)
            {
                if (scriptEnum.name == enumName)
                {
                    rhs = scriptEnum;
                }
            }
            rhs.selected = node[1].as<std::size_t>();

            return true;
        }
    };


    template<>
    struct convert<Pin>
    {
        static Node encode(const Pin& rhs)
        {
            Node node;

            node.push_back(rhs.scopes);
            node.push_back((int)rhs.pinKind);
            return node;
        }

        static bool decode(const Node& node, Pin& rhs)
        {
            if (!node.IsSequence() && node.size() == 2)
                return false;

            rhs = Pin((ed::PinKind)node[1].as<int>(), node[0].as<scopeKey>());

            return true;
        }
    };

}
