#pragma once
#include <functional>

typedef unsigned int FMOD_RESULT;
typedef unsigned int FMOD_SPEAKERMODE;
typedef unsigned int FMOD_STUDIO_INITFLAGS;
typedef unsigned int FMOD_INITFLAGS;
typedef unsigned int FMOD_STUDIO_LOAD_BANK_FLAGS;
typedef unsigned int FMOD_STUDIO_STOP_MODE;


//typedef enum FMOD_RESULT
//{
//    FMOD_OK,
//    FMOD_ERR_BADCOMMAND,
//    FMOD_ERR_CHANNEL_ALLOC,
//    FMOD_ERR_CHANNEL_STOLEN,
//    FMOD_ERR_DMA,
//    FMOD_ERR_DSP_CONNECTION,
//    FMOD_ERR_DSP_DONTPROCESS,
//    FMOD_ERR_DSP_FORMAT,
//    FMOD_ERR_DSP_INUSE,
//    FMOD_ERR_DSP_NOTFOUND,
//    FMOD_ERR_DSP_RESERVED,
//    FMOD_ERR_DSP_SILENCE,
//    FMOD_ERR_DSP_TYPE,
//    FMOD_ERR_FILE_BAD,
//    FMOD_ERR_FILE_COULDNOTSEEK,
//    FMOD_ERR_FILE_DISKEJECTED,
//    FMOD_ERR_FILE_EOF,
//    FMOD_ERR_FILE_ENDOFDATA,
//    FMOD_ERR_FILE_NOTFOUND,
//    FMOD_ERR_FORMAT,
//    FMOD_ERR_HEADER_MISMATCH,
//    FMOD_ERR_HTTP,
//    FMOD_ERR_HTTP_ACCESS,
//    FMOD_ERR_HTTP_PROXY_AUTH,
//    FMOD_ERR_HTTP_SERVER_ERROR,
//    FMOD_ERR_HTTP_TIMEOUT,
//    FMOD_ERR_INITIALIZATION,
//    FMOD_ERR_INITIALIZED,
//    FMOD_ERR_INTERNAL,
//    FMOD_ERR_INVALID_FLOAT,
//    FMOD_ERR_INVALID_HANDLE,
//    FMOD_ERR_INVALID_PARAM,
//    FMOD_ERR_INVALID_POSITION,
//    FMOD_ERR_INVALID_SPEAKER,
//    FMOD_ERR_INVALID_SYNCPOINT,
//    FMOD_ERR_INVALID_THREAD,
//    FMOD_ERR_INVALID_VECTOR,
//    FMOD_ERR_MAXAUDIBLE,
//    FMOD_ERR_MEMORY,
//    FMOD_ERR_MEMORY_CANTPOINT,
//    FMOD_ERR_NEEDS3D,
//    FMOD_ERR_NEEDSHARDWARE,
//    FMOD_ERR_NET_CONNECT,
//    FMOD_ERR_NET_SOCKET_ERROR,
//    FMOD_ERR_NET_URL,
//    FMOD_ERR_NET_WOULD_BLOCK,
//    FMOD_ERR_NOTREADY,
//    FMOD_ERR_OUTPUT_ALLOCATED,
//    FMOD_ERR_OUTPUT_CREATEBUFFER,
//    FMOD_ERR_OUTPUT_DRIVERCALL,
//    FMOD_ERR_OUTPUT_FORMAT,
//    FMOD_ERR_OUTPUT_INIT,
//    FMOD_ERR_OUTPUT_NODRIVERS,
//    FMOD_ERR_PLUGIN,
//    FMOD_ERR_PLUGIN_MISSING,
//    FMOD_ERR_PLUGIN_RESOURCE,
//    FMOD_ERR_PLUGIN_VERSION,
//    FMOD_ERR_RECORD,
//    FMOD_ERR_REVERB_CHANNELGROUP,
//    FMOD_ERR_REVERB_INSTANCE,
//    FMOD_ERR_SUBSOUNDS,
//    FMOD_ERR_SUBSOUND_ALLOCATED,
//    FMOD_ERR_SUBSOUND_CANTMOVE,
//    FMOD_ERR_TAGNOTFOUND,
//    FMOD_ERR_TOOMANYCHANNELS,
//    FMOD_ERR_TRUNCATED,
//    FMOD_ERR_UNIMPLEMENTED,
//    FMOD_ERR_UNINITIALIZED,
//    FMOD_ERR_UNSUPPORTED,
//    FMOD_ERR_VERSION,
//    FMOD_ERR_EVENT_ALREADY_LOADED,
//    FMOD_ERR_EVENT_LIVEUPDATE_BUSY,
//    FMOD_ERR_EVENT_LIVEUPDATE_MISMATCH,
//    FMOD_ERR_EVENT_LIVEUPDATE_TIMEOUT,
//    FMOD_ERR_EVENT_NOTFOUND,
//    FMOD_ERR_STUDIO_UNINITIALIZED,
//    FMOD_ERR_STUDIO_NOT_LOADED,
//    FMOD_ERR_INVALID_STRING,
//    FMOD_ERR_ALREADY_LOCKED,
//    FMOD_ERR_NOT_LOCKED,
//    FMOD_ERR_RECORD_DISCONNECTED,
//    FMOD_ERR_TOOMANYSAMPLES
//} FMOD_RESULT;


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

    class System
    {
    public:
        static std::function<FMOD_RESULT(System* thisClass, int samplerate, FMOD_SPEAKERMODE speakermode, int numrawspeakers)> setSoftwareFormat;
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