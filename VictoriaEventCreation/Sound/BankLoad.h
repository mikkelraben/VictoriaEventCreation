#pragma once
namespace Bank
{
    struct Packet
    {
        int offset = 0;
        int granulePosition = 0;
    };

    class Sound
    {
    public:
        std::string name;
        int offset;
        int samples;
        int channels;
        unsigned int crc32Hash;
        std::vector<Packet> pages;
    };

    class File
    {
    public:
        explicit File(const std::filesystem::path newPath) : path{ newPath } {};
        std::filesystem::path path;
        std::mutex soundsMutex;
        std::vector<Sound> sounds;
        void ParseBankFile();
        long long startData;
    private:
        void ReadWavs(std::basic_ifstream<std::byte>& file);
        void ReadSoundHeader(std::basic_ifstream<std::byte>& file);
        void ReadNumberOfWavs(std::basic_ifstream<std::byte>& file);
    };
}
