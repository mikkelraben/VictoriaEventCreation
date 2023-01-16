#include "../Core/pch.h"
#include <SFML/Audio.hpp>
#include <vorbis/vorbisenc.h>
#include "BankLoad.h"
#include <Windows.h>

namespace Sound
{
    std::vector<Event> SoundSystem::events = {};

    FMOD::Studio::System*  SoundSystem::system = nullptr;
    FMOD::System*  SoundSystem::coreSystem = nullptr;

    std::vector<FMOD::Studio::Bank*> SoundSystem::banks = {};
    std::vector<FMOD::Studio::EventDescription*> SoundSystem::eventVector = {};


    void SoundSystem::InitSoundSystem()
    {
        CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
        FMOD::FunctionLoader functionLoader;
        functionLoader.loadFunctions();

        
        RE_ASSERT(FMOD::Studio::System::Create(&system, 0x00020100));
        
        
        RE_ASSERT(system->getCoreSystem(system, &coreSystem));

        RE_ASSERT(coreSystem->setSoftwareFormat(coreSystem, 0, 2, 0));

        RE_ASSERT(system->intialize(system, 1024, 0, 0, nullptr));

        setVolume(Settings::volume.getSetting());

        std::filesystem::path banksFolder = Settings::gameDirectory.getSetting() / "game\\sound\\banks";

        for (const std::filesystem::directory_entry& dir_entry : std::filesystem::directory_iterator{ banksFolder })
        {
            FMOD::Studio::Bank* newBank = nullptr;
            std::string file = dir_entry.path().filename().string();
            if (file == "Soundtrack.bank")
            {
                continue;
            }

            if (dir_entry.exists())
            {
                std::string fileName = dir_entry.path().generic_string();
                FMOD_RESULT err = system->loadBankFile(system, fileName.c_str(), 0, &newBank);
                if (err != 0)
                {
                    RE_LogError("Could Not Load Bank: " + fileName);
                }
                if (newBank)
                {
                    banks.push_back(newBank);
                    RE_LogMessage("Loaded: " + dir_entry.path().string());
                }
            }
        }

        for (auto& bank : banks)
        {
            int eventCount;
            RE_ASSERT(bank->getEventCount(bank, &eventCount));
            if (eventCount == 0)
            {
                continue;
            }
            eventVector = { (size_t)eventCount, nullptr };

            int eventsWritten;
            RE_ASSERT(bank->getEventList(bank, eventVector.data(), eventVector.size(), &eventsWritten));

            for (auto& eventDescription : eventVector)
            {
                char buffer[256];
                int stringSize;
                RE_ASSERT(eventDescription->getPath(eventDescription, buffer, 256, &stringSize));
                std::string eventName = { buffer };

                FMOD::Studio::EventInstance* instance = nullptr;
                RE_ASSERT(eventDescription->createInstance(eventDescription,&instance));

                events.push_back({ eventName, instance});
            }

        }
    }

    void SoundSystem::Update()
    {
        RE_ASSERT(system->update(system));
    }

    void SoundSystem::DeleteSoundSystem()
    {
        for (auto& bank : banks)
        {
            bank->unload(bank);
        }

        system->release(system);
    }

    void SoundSystem::setVolume(float volume)
    {
        FMOD::ChannelGroup* channelGroup;
        coreSystem->getMasterChannelGroup(coreSystem, &channelGroup);
        channelGroup->setVolume(channelGroup, volume);
    }

    

    void Event::Play()
    {
        RE_ASSERT(instance->start(instance));
    }
    void Event::Stop()
    {
        RE_ASSERT(instance->stop(instance, 1));
    }
}