#include "../Core/pch.h"
#include <SFML/Audio.hpp>
#include <vorbis/vorbisenc.h>
#include "BankLoad.h"

using namespace std::chrono_literals;

namespace Bank
{
    struct ListItem
    {
        char List[5] = "";
        unsigned int length = 0;
    };

    struct ChunkBitField
    {
        unsigned int notLast : 1;
        unsigned int chunksize : 24;
        unsigned int type : 7;
    };

    struct SoundHeaderBitField
    {
        unsigned long long extraParameters : 1;
        unsigned long long frequency : 4;
        unsigned long long twoChannels : 1;
        unsigned long long offset : 28;
        unsigned long long samples : 30;
    };

    void File::ReadNumberOfWavs(std::basic_ifstream<std::byte>& file)
    {
        int numberOfSounds;
        file.seekg(12 + file.tellg());
        file.read(reinterpret_cast<std::byte*>(&numberOfSounds), 4);
        RE_LogMessage("No. of Sound Effects: " + std::to_string(numberOfSounds));

        sounds.resize(numberOfSounds);

    }

    void File::ReadSoundHeader(std::basic_ifstream<std::byte>& file)
    {
        for (size_t i = 0; i < sounds.size(); i++)
        {
            Sound soundDetails;

            SoundHeaderBitField headerBitField;

            file.read(reinterpret_cast<std::byte*>(&headerBitField), 8);

            soundDetails.offset = headerBitField.offset;
            soundDetails.samples = headerBitField.samples;
            soundDetails.channels = headerBitField.twoChannels + 1;

            if (headerBitField.extraParameters)
            {
                ChunkBitField chunkBitField;
                do
                {
                    file.read(reinterpret_cast<std::byte*>(&chunkBitField), 4);
                    if (chunkBitField.type != 11 && chunkBitField.notLast)
                    {
                        file.seekg(chunkBitField.chunksize + file.tellg());
                    }
                    else
                    {
                        //skip crc32 hash
                        if (chunkBitField.type == 11)
                        {
                            file.read(reinterpret_cast<std::byte*>(&soundDetails.crc32Hash), 4);
                        }
                        else
                        {
                            file.seekg(4 + file.tellg());
                        }

                        int numberOfPackets = chunkBitField.chunksize / 8;

                        for (size_t i = 0; i < numberOfPackets; i++)
                        {
                            Packet packet;

                            bool isLast = (i + 1) == numberOfPackets;

                            file.read(reinterpret_cast<std::byte*>(&packet), isLast ? 4 : 8);

                            soundDetails.pages.push_back(packet);
                        }
                    }

                } while (chunkBitField.notLast);
            }

            sounds[i] = soundDetails;
        }


    }

    void File::ReadWavs(std::basic_ifstream<std::byte>& file)
    {
        int soundFormat = 0;
        int HeaderSize = 0;

        file.read(reinterpret_cast<std::byte*>(&HeaderSize), 4);


        file.seekg(8 + file.tellg());
        file.read(reinterpret_cast<std::byte*>(&soundFormat), 4);

        if (soundFormat != 15)
        {
            std::cout << "Error: Format Does not fit \n";
        }

        file.seekg(32 + file.tellg());

        ReadSoundHeader(file);

        //TODO: Split into own function
        //names of sound effects
        const int stringSize = 128;
        long long startArray = file.tellg();

        for (size_t i = 0; i < sounds.size(); i++)
        {
            file.seekg(i * 4 + startArray);

            int stringStartOffset;
            file.read(reinterpret_cast<std::byte*>(&stringStartOffset), 4);

            file.seekg(startArray + stringStartOffset);

            auto nameBuffer = std::make_unique<char[]>(stringSize);

            //read name
            file.getline(reinterpret_cast<std::byte*>(nameBuffer.get()), stringSize, std::byte(0));


            std::string soundName = nameBuffer.get();
            std::scoped_lock lock(soundsMutex);
            sounds[i].name = soundName;
        }
    }

    void File::ParseBankFile()
    {
        if (!path.has_filename())
        {
            return;
        }
        std::string filename = path.filename().string();

        //find length of file
        std::basic_ifstream<std::byte> file;
        file.open(filename, std::ios::binary | std::ios::ate);
        long long fileSize = file.tellg();
        file.close();


        //open file
        file.open(filename, std::ios::binary);


        if (file.is_open())
        {
            RE_LogMessage(filename + " opened");

            auto start = std::chrono::high_resolution_clock::now();

            ListItem header;

            file.read(reinterpret_cast<std::byte*>(&header.List), 4);
            file.read(reinterpret_cast<std::byte*>(&header.length), 4);

            bool endList = false;
            file.seekg(36);

            int i = 0;

            while (!endList)
            {
                long long position = file.tellg();

                if (position == fileSize || i == 100)
                {
                    break;
                }

                ListItem listitem;
                file.read(reinterpret_cast<std::byte*>(&listitem.List), 4);
                file.read(reinterpret_cast<std::byte*>(&listitem.length), 4);

                position = file.tellg();

                if (i == 26)
                {
                    ReadNumberOfWavs(file);
                }

                if (strcmp("PROJ", listitem.List) == 0)
                {
                    file.seekg(36 + position);
                }
                else if (strcmp("SND ", listitem.List) == 0)
                {
                    file.seekg(4 + position);
                    char firstLetter = 0;

                    //seek to FSB5
                    do
                    {
                        file.read(reinterpret_cast<std::byte*>(&firstLetter), 1);
                    } while (firstLetter != 'F');

                    file.seekg(3 + 8 + file.tellg());

                    ReadWavs(file);
                    endList = true;
                }
                else
                {
                    if (listitem.length == 0)
                    {
                        //file.seekg(4 + position);
                    }
                    else
                    {
                        file.seekg(listitem.length + position);
                    }
                }



                i++;
            }

            //decode the vorbis blocks to raw audio data
            startData = file.tellg();

            int filter = 0xFFFFFFC0;

            startData = startData & filter;
            startData += 64;

            file.seekg(startData);

            file.close();

            auto end = std::chrono::high_resolution_clock::now();

            std::chrono::duration<double> delta = end - start;

            RE_LogMessage("time: " + std::to_string(delta.count()) + "s");
        }
    }
}