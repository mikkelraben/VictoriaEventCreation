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
    Param(Sound::Event startValue, std::string const& _name) { variable = startValue; name = _name; if (startValue.instance) { FindSelection(); } }
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
};
