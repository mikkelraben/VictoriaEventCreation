#include "pch.h"
#include "../Core/pch.h"
#include "FMOD.h"

#include <Windows.h>

//Core API
//System
std::function<FMOD_RESULT(FMOD::System* thisClass, int samplerate, FMOD_SPEAKERMODE speakermode, int numrawspeakers)> FMOD::System::setSoftwareFormat = [](System* thisClass, int samplerate, FMOD_SPEAKERMODE speakermode, int numrawspeakers) { return 1; };
std::function<FMOD_RESULT(FMOD::System* thisClass, FMOD::ChannelGroup** channelGroup)> FMOD::System::getMasterChannelGroup = [](System* thisClass, ChannelGroup** channelGroup) { return 1; };

//ChannelGroup
std::function<FMOD_RESULT(FMOD::ChannelGroup* thisClass, float volume)> FMOD::ChannelGroup::setVolume = [](ChannelGroup* thisClass, float volume) { return 1; };


//Studio API
//System
std::function<FMOD_RESULT(FMOD::Studio::System** system, unsigned int headerversion)> FMOD::Studio::System::Create =                                                                            [](System** system, unsigned int headerversion) { return 1; };
std::function<FMOD_RESULT(FMOD::Studio::System* system)> FMOD::Studio::System::release =                                                                                                        [](System* system) { return 1; };
std::function<FMOD_RESULT(FMOD::Studio::System* system)> FMOD::Studio::System::update =                                                                                                         [](System* system) { return 1; };
std::function<FMOD_RESULT(FMOD::Studio::System* thisClass, FMOD::System** system)> FMOD::Studio::System::getCoreSystem =                                                                        [](System*, FMOD::System**) { return 1; };
std::function<FMOD_RESULT(FMOD::Studio::System* thisClass, int maxChannels, FMOD_STUDIO_INITFLAGS studioFlags, FMOD_INITFLAGS flags, void* extraDriverData)> FMOD::Studio::System::intialize =  [](System* thisClass, int maxChannels, FMOD_STUDIO_INITFLAGS studioFlags, FMOD_INITFLAGS flags, void* extraDriverData) { return 1; };
std::function<FMOD_RESULT(FMOD::Studio::System* thisClass, const char* filename, FMOD_STUDIO_LOAD_BANK_FLAGS flags, FMOD::Studio::Bank** bank)> FMOD::Studio::System::loadBankFile =            [](System*, const char* filename, FMOD_STUDIO_LOAD_BANK_FLAGS flags, FMOD::Studio::Bank** bank) { return 1; };
std::function<FMOD_RESULT(FMOD::Studio::System* thisClass, const char* path, FMOD::Studio::EventDescription** eventDescription)> FMOD::Studio::System::getEvent =                               [](System* thisClass, const char* path, EventDescription** eventDescription) { return 1; };

//Bank
std::function<FMOD_RESULT(FMOD::Studio::Bank* bank)> FMOD::Studio::Bank::unload =                                                                               [](Bank* bank) { return 1; };
std::function<FMOD_RESULT(FMOD::Studio::Bank* bank, int* count)> FMOD::Studio::Bank::getEventCount =                                                            [](Bank* bank, int* count) { return 1; };
std::function<FMOD_RESULT(FMOD::Studio::Bank* bank, FMOD::Studio::EventDescription** eventsArray, int capacity, int* count)> FMOD::Studio::Bank::getEventList = [](Bank* bank, EventDescription** eventsArray, int capacity, int* count) { return 1; };

//EventDescription
std::function<FMOD_RESULT(FMOD::Studio::EventDescription* thisClass, char* path, int size, int* retrieved)> FMOD::Studio::EventDescription::getPath =           [](EventDescription* thisClass, char* path, int size, int* retrieved) { return 1; };
std::function<FMOD_RESULT(FMOD::Studio::EventDescription* thisClass, FMOD::Studio::EventInstance** instance)> FMOD::Studio::EventDescription::createInstance =  [](EventDescription* thisClass, EventInstance** instance) { return 1; };

//EventInstance
std::function<FMOD_RESULT(FMOD::Studio::EventInstance* thisClass)> FMOD::Studio::EventInstance::start = [](EventInstance* thisClass) { return 1; };
std::function<FMOD_RESULT(FMOD::Studio::EventInstance* thisClass, FMOD_STUDIO_STOP_MODE mode)> FMOD::Studio::EventInstance::stop = [](EventInstance* thisClass, FMOD_STUDIO_STOP_MODE mode) { return 1; };


HINSTANCE loadDLL(std::filesystem::path& libraryPath)
{
    if (!std::filesystem::exists(libraryPath))
    {
        RE_LogError("Could not load DLL, library does not exist. Try updating game location");

        return nullptr;
    }
    std::string library = libraryPath.string();
    HINSTANCE dllAddress = LoadLibraryA(library.c_str());

    // Check if DLL is loaded.
    if (dllAddress == NULL)
    {
        DWORD error = GetLastError();
        RE_LogError("Could not load DLL \"" + library + "\" Error: " + std::to_string(error));
        return nullptr;
    }


    return dllAddress;
}

template <typename Return, typename... Arguments>
std::function<Return(Arguments...)> LoadFunction(const std::string& functionName, HINSTANCE dllHandle)
{
    FARPROC FunctionAddress = GetProcAddress(dllHandle, functionName.c_str());

    if (!FunctionAddress)
    {
        DWORD error = GetLastError();

        RE_LogError("Could not load Function \"" + functionName + "\" Error: " + std::to_string(error));
    }
    return std::function<Return(Arguments...)>(reinterpret_cast<Return(__stdcall*)(Arguments...)>(FunctionAddress));

}

bool FMOD::FunctionLoader::loadFunctions()
{
    std::filesystem::path libraryPath = Settings::gameDirectory.getSetting() / "binaries\\fmod.dll";
    std::filesystem::path studioLibraryPath = Settings::gameDirectory.getSetting() / "binaries\\fmodstudio.dll";

    HINSTANCE coreDLL = nullptr;
    //try loading dlls
    if (!(coreDLL = loadDLL(libraryPath)))
    {
        return false;
    }
    HINSTANCE studioDLL = nullptr;
    if (!(studioDLL = loadDLL(studioLibraryPath)))
    {
        return false;
    }

    //Core System
    System::setSoftwareFormat = LoadFunction<FMOD_RESULT, System*, int, FMOD_SPEAKERMODE, int>("FMOD_System_SetSoftwareFormat", coreDLL);
    System::getMasterChannelGroup = LoadFunction<FMOD_RESULT, System*, ChannelGroup**>("FMOD_System_GetMasterChannelGroup", coreDLL);

    //Core ChannelGroup
    ChannelGroup::setVolume = LoadFunction<FMOD_RESULT, ChannelGroup*, float>("FMOD_ChannelGroup_SetVolume", coreDLL);

    //Studio System
    Studio::System::Create = LoadFunction<FMOD_RESULT, Studio::System**, unsigned int>("FMOD_Studio_System_Create", studioDLL);
    Studio::System::release = LoadFunction<FMOD_RESULT, Studio::System*>("FMOD_Studio_System_Release", studioDLL);
    Studio::System::update = LoadFunction<FMOD_RESULT, Studio::System*>("FMOD_Studio_System_Update", studioDLL);
    Studio::System::getCoreSystem = LoadFunction<FMOD_RESULT, Studio::System*, System**>("FMOD_Studio_System_GetCoreSystem", studioDLL);
    Studio::System::intialize = LoadFunction<FMOD_RESULT, Studio::System*, int, FMOD_STUDIO_INITFLAGS, FMOD_INITFLAGS, void*>("FMOD_Studio_System_Initialize", studioDLL);
    Studio::System::loadBankFile = LoadFunction<FMOD_RESULT, Studio::System*, const char*, FMOD_STUDIO_LOAD_BANK_FLAGS, Studio::Bank**>("FMOD_Studio_System_LoadBankFile", studioDLL);
    Studio::System::getEvent = LoadFunction<FMOD_RESULT, Studio::System*, const char*, Studio::EventDescription**>("FMOD_Studio_System_GetEvent", studioDLL);

    //Studio Bank
    Studio::Bank::unload = LoadFunction<FMOD_RESULT, Studio::Bank*>("FMOD_Studio_Bank_Unload", studioDLL);
    Studio::Bank::getEventCount = LoadFunction<FMOD_RESULT, Studio::Bank*, int*>("FMOD_Studio_Bank_GetEventCount", studioDLL);
    Studio::Bank::getEventList = LoadFunction<FMOD_RESULT, Studio::Bank*, Studio::EventDescription**, int, int*>("FMOD_Studio_Bank_GetEventList", studioDLL);

    //Studio EventDescription
    Studio::EventDescription::getPath = LoadFunction<FMOD_RESULT, Studio::EventDescription*, char*, int, int*>("FMOD_Studio_EventDescription_GetPath", studioDLL);
    Studio::EventDescription::createInstance = LoadFunction<FMOD_RESULT, Studio::EventDescription*, Studio::EventInstance**>("FMOD_Studio_EventDescription_CreateInstance", studioDLL);

    //Studio EventInstance
    Studio::EventInstance::start = LoadFunction<FMOD_RESULT, Studio::EventInstance*>("FMOD_Studio_EventInstance_Start", studioDLL);
    Studio::EventInstance::stop = LoadFunction<FMOD_RESULT, Studio::EventInstance*, FMOD_STUDIO_STOP_MODE>("FMOD_Studio_EventInstance_Stop", studioDLL);


    return true;
}

void FMOD::FunctionLoader::freeFunctions()
{
}
