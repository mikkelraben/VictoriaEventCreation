#include "pch.h"
#include "../Core/pch.h"
#include "Sound.h"
#include <SFML/Audio.hpp>
#include <ogg/ogg.h>
#include <vorbis/codec.h>

using namespace std::chrono_literals;


void Sound::playSound()
{
    sf::InputSoundFile buffer2;

    bool test = buffer2.openFromMemory(&fileInMemory[0], fileInMemory.size());


    if (buffer.loadFromMemory(&fileInMemory[0], fileInMemory.size()))
    {
        soundPlayer.setBuffer(buffer);
        soundPlayer.play();
    }
}

//channels = number of audio channels
//sampleRate = frequency of samples in Hz
Ogg_Packet CreateIdHeader(int channels, int sampleRate)
{
    const long packetSize = 30;
    unsigned char* packetData = new unsigned char[packetSize];

    memset(packetData, 0, packetSize);

    const char* vorbisStart = "\01vorbis";

    //
    memcpy(packetData, vorbisStart, 7);
    //7..10 vorbis_version

    //11 audio_channels
    packetData[11] = channels;

    //12 audio_sample_rate
    *reinterpret_cast<int*>(&packetData[12]) = sampleRate;

    //13..27 bitrate_maximum bitrate_nominal bitrate_minimum

    //28 blocksize_0 and blocksize_1
    packetData[28] = 184;

    //29 framing flag
    packetData[29] = 1;

    //did not work without new ?
    Ogg_Packet packet;

    packet->b_o_s = 1;
    packet->e_o_s = 0;
    packet->granulepos = 0;
    packet->packetno = 0;

    packet->packet = packetData;
    packet->bytes = packetSize;

    return packet;

}

Ogg_Packet CreateCommentsHeader()
{
    Ogg_Packet packet;

    vorbis_comment comment;

    vorbis_comment_init(&comment);

    vorbis_commentheader_out(&comment, &packet);

    return packet;
}

Ogg_Packet CreateSetupHeader(unsigned int hashToFind)
{
    std::basic_ifstream<unsigned char> file;

    file.open("vorbisHeaders", std::ios::binary);

    unsigned char* copyHeaderBuffer = nullptr;



    long headerLength = 0;

    if (file.is_open())
    {
        bool NotFound = true;
        int i = 0;
        while (NotFound)
        {
            int hash = 0;
            int length = 0;
            file.read(reinterpret_cast<unsigned char*>(&hash), 4);
            file.read(reinterpret_cast<unsigned char*>(&length), 4);

            if (hash == hashToFind)
            {
                //std::cout << "Found at:" << file.tellg();
                copyHeaderBuffer = new unsigned char[length];
                file.read(copyHeaderBuffer, length);

                headerLength = length;
                NotFound = false;
            }
            else
            {
                file.seekg(length + file.tellg());
            }
            i++;
            if (i > 161)
            {
                break;
            }
        }

        file.close();
    }

    Ogg_Packet packet;

    packet->packet = copyHeaderBuffer;
    packet->bytes = headerLength;

    packet->b_o_s = 0;
    packet->e_o_s = 0;
    packet->granulepos = 0;
    packet->packetno = 2;

    return packet;
}

void Sound::SubmitData(Ogg_stream_state& state, bool flush)
{
    Ogg_Page page;
    //if flush then flush else default page logic
    while (flush ? ogg_stream_flush(&state,&page) : ogg_stream_pageout(&state, &page))
    {
        std::vector<char> header(page->header, page->header + page->header_len);
        std::vector<char> body(page->body, page->body + page->body_len);

        fileInMemory.insert(fileInMemory.end(), header.begin(), header.end());
        fileInMemory.insert(fileInMemory.end(), body.begin(), body.end());

        //fileOut.write((char*)page->header, page->header_len);
        //fileOut.write((char*)page->body, page->body_len);
    }
}

void Sound::play()
{
    if (fileInMemory.empty())
    {
        PlayingThread = std::thread(&Sound::createVorbisFile,this);
    }
    PlayingThread.join();
    PlayingThread = std::thread(&Sound::playSound, this);
    PlayingThread.join();
}

void Sound::createVorbisFile()
{
    auto start = std::chrono::high_resolution_clock::now();


    //create an ogg stream to create pages from raw data
    Ogg_stream_state stream;

    //need vorbis for granuleposition
    Vorbis_info vorbisInfo;
    Vorbis_comment vorbisComment;


    Ogg_Packet idHeader = CreateIdHeader(sound.channels, 48000);
    Ogg_Packet commentsHeader = CreateCommentsHeader();
    Ogg_Packet setupHeader = CreateSetupHeader(sound.crc32Hash);

    vorbis_synthesis_headerin(&vorbisInfo, &vorbisComment, &idHeader);
    vorbis_synthesis_headerin(&vorbisInfo, &vorbisComment, &commentsHeader);
    vorbis_synthesis_headerin(&vorbisInfo, &vorbisComment, &setupHeader);

    ogg_stream_packetin(&stream, &idHeader);
    SubmitData(stream, false);
    ogg_stream_packetin(&stream, &commentsHeader);
    SubmitData(stream, false);
    ogg_stream_packetin(&stream, &setupHeader);
    SubmitData(stream, false);
    SubmitData(stream, true);


    std::basic_ifstream<std::byte> bankFile;
    bankFile.open(file.path, std::ios::binary);
    if (bankFile.is_open())
    {
        //seek to correct position in file
        size_t lastPageOffset = sound.pages[sound.pages.size() - 1].offset;
        int soundStartOffset = file.startData + sound.offset * 16;
        bankFile.seekg(soundStartOffset);


        int granulePosition = 0;

        int packetStart = ((ogg_stream_state)stream).packetno;
        int packetNumber = 0;
        int prevBlockSize = 0;
        bool nextPacketLast = false;
        bool packetsAvailable = true;
        while (packetsAvailable)
        {
            //packet is defined by this
            //**   *          *...
            //size packettype data


            packetNumber++;
            Ogg_Packet packet;

            int pos = bankFile.tellg();


            //read size
            unsigned short bytes = 0;
            bankFile.read((std::byte*)&bytes, 2);

            pos = bankFile.tellg();

            //define packetData and fill with data
            auto packetData = new unsigned char[bytes];
            bankFile.read((std::byte*)packetData, bytes);

            int currentOffset = -soundStartOffset + bankFile.tellg();

            pos = bankFile.tellg();

            if (nextPacketLast)
            {
                packetsAvailable = false;
            }

            if (currentOffset > lastPageOffset)
            {
                nextPacketLast = true;
            }


            packet->bytes = bytes;
            packet.packet.packetno = packetNumber + packetStart;
            packet->packet = packetData;
            packet->b_o_s = 0;
            packet->e_o_s = !packetsAvailable;

            int blockSize = vorbis_packet_blocksize(&vorbisInfo, &packet);
            if (prevBlockSize)
            {
                granulePosition = granulePosition + (blockSize + prevBlockSize) / 4;
            }
            else
            {
                granulePosition = 0;
            }
            packet->granulepos = granulePosition;
            prevBlockSize = blockSize;

            ogg_stream_packetin(&stream, &packet);
            SubmitData(stream, false);

        }
    }

    auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> delta = end - start;

    RE_LogMessage("sound time: " + std::to_string(delta.count()) + "s");

}
