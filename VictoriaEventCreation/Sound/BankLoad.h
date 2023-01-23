#pragma once
#include "FMOD.h"
#include "yaml-cpp/yaml.h"

namespace Sound
{
    class Event
    {
    public:
        Event() = default;
        Event(std::string string, FMOD::Studio::EventInstance* sound) { name = string; instance = sound; };
        Event(const std::string& string);
        std::string name;
        FMOD::Studio::EventInstance* instance;
        void Play();
        void Stop();
    private:

    };

    class SoundSystem
    {
    public:
        static std::vector<Event> events;
        static void InitSoundSystem();
        static void DeleteSoundSystem();
        static void setVolume(float volume);

        //call every frame
        static void Update();

        static FMOD::Studio::System* system;
        static FMOD::System* coreSystem;
        static std::vector<FMOD::Studio::Bank*> banks;
        static std::vector<FMOD::Studio::EventDescription*> eventVector;
    };
}

namespace YAML
{
    template<>
    struct convert<Sound::Event>
    {
        static Node encode(const Sound::Event& rhs)
        {
            Node node;

            node.push_back(rhs.name);
            return node;
        }

        static bool decode(const Node& node, Sound::Event& rhs)
        {
            if (!node.IsSequence() && node.size() == 1)
                return false;
            rhs = { node[0].as<std::string>()};
            return true;
        }
    };
}
