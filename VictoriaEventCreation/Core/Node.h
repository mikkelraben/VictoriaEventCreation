#pragma once
#include "yaml-cpp/yaml.h"
#include "../Sound/BankLoad.h"

struct BasicNode
{
    std::string name;
    virtual void EditableField() { ImGui::Text("Not Implemented"); };
    virtual YAML::Node Serialize() = 0;
    virtual void Deserialize(const YAML::Node& node) = 0;
    virtual ~BasicNode() = default;
    static BasicNode* findChildFromName(std::vector<BasicNode*>& children, std::string_view name);
};

struct Node : public BasicNode
{
    Node(std::string_view _name, std::string_view _formalName) : formalName(_formalName) { };
    int possibleScope = 0;
    std::string formalName;
    std::vector<BasicNode*> children;
    void EditableField() override;
    YAML::Node Serialize() override;
    void Deserialize(const YAML::Node& node) override;
};

template<typename T>
struct Param : public BasicNode
{
    Param(T startValue, std::string const& _name) { variable = startValue; name = _name; }
    T variable;
    void EditableField() override;
    YAML::Node Serialize() override;
    void Deserialize(const YAML::Node& node) override
    {
        name = node["name"].as<std::string>();
        variable = node["variable"].as<T>();
    };
};

template<>
struct Param<Sound::Event> : public BasicNode
{
    Param(Sound::Event startValue, std::string const& _name, bool _ui, bool _event) { variable = startValue; name = _name; if (startValue.instance) { FindSelection(); } ui = _ui; event = _event;}
    Sound::Event variable;
    void EditableField() override;
    YAML::Node Serialize() override;
    void Deserialize(const YAML::Node& node) override
    {
        name = node["name"].as<std::string>();
        variable = node["variable"].as<Sound::Event>();
    };
private:
    void FindSelection();
    std::string preview = "Pick A Sound";
    Sound::SoundSystem soundSystem;
    int selected = 0;
    bool ui = false, event = false;
};
