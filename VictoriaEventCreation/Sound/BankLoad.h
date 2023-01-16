#pragma once
#include "FMOD.h"

namespace Sound
{
    class Event
    {
    public:
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

        //call every frame
        static void Update();

        static FMOD::Studio::System* system;
        static FMOD::System* coreSystem;
        static std::vector<FMOD::Studio::Bank*> banks;
        static std::vector<FMOD::Studio::EventDescription*> eventVector;
    };
}
