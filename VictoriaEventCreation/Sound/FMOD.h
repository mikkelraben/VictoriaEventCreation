#pragma once
#include <functional>

typedef unsigned int FMOD_RESULT;
typedef unsigned int FMOD_SPEAKERMODE;
typedef unsigned int FMOD_STUDIO_INITFLAGS;
typedef unsigned int FMOD_INITFLAGS;
typedef unsigned int FMOD_STUDIO_LOAD_BANK_FLAGS;
typedef unsigned int FMOD_STUDIO_STOP_MODE;

namespace FMOD
{
    class FunctionLoader
    {
    public:
        bool loadFunctions();
        ~FunctionLoader() { freeFunctions(); };

    private:
        void freeFunctions();
    };

    class ChannelGroup
    {
    public:
        static std::function<FMOD_RESULT(ChannelGroup* thisClass, float volume)> setVolume;
    };

    class System
    {
    public:
        static std::function<FMOD_RESULT(System* thisClass, int samplerate, FMOD_SPEAKERMODE speakermode, int numrawspeakers)> setSoftwareFormat;
        static std::function<FMOD_RESULT(System* thisClass, ChannelGroup** channelGroup)> getMasterChannelGroup;
    };

    namespace Studio
    {
        class EventInstance
        {
        public:
            static std::function<FMOD_RESULT(EventInstance* thisClass)> start;
            static std::function<FMOD_RESULT(EventInstance* thisClass, FMOD_STUDIO_STOP_MODE mode)> stop;
        };

        class EventDescription
        {
        public:
            static std::function<FMOD_RESULT(EventDescription* thisClass, char* path, int size, int* retrieved)> getPath;
            static std::function<FMOD_RESULT(EventDescription* thisClass, EventInstance** instance)> createInstance;
        };

        class Bank
        {
        public:
            static std::function<FMOD_RESULT(Bank* bank)> unload;
            static std::function<FMOD_RESULT(Bank* bank, int* count)> getEventCount;
            static std::function<FMOD_RESULT(Bank* bank, EventDescription** eventsArray, int capacity, int* count)> getEventList;
        };

        class System
        {
        public:
            static std::function<FMOD_RESULT(System** system, unsigned int headerversion)> Create;
            static std::function<FMOD_RESULT(System* thisClass)> release;
            static std::function<FMOD_RESULT(System* thisClass)> update;
            static std::function<FMOD_RESULT(System* thisClass, FMOD::System** system)> getCoreSystem;
            static std::function<FMOD_RESULT(System* thisClass, int maxChannels, FMOD_STUDIO_INITFLAGS studioFlags, FMOD_INITFLAGS flags, void* extraDriverData)> intialize;
            static std::function<FMOD_RESULT(System* thisClass, const char* filename, FMOD_STUDIO_LOAD_BANK_FLAGS flags, Bank** bank)> loadBankFile;
            static std::function<FMOD_RESULT(System* thisClass, const char* path, EventDescription** eventDescription)> getEvent;
        };



    }

}