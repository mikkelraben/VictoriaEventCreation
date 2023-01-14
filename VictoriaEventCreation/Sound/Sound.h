#pragma once
#include "Ogg.h"
#include "BankLoad.h"
#include <SFML/Audio.hpp>

class Sound
{
public:
    Sound(Bank::Sound& _sound, Bank::File& _file) : file(_file), sound(_sound) {}//{ fileOut.open("test.ogg", std::ios::binary); }
    void play();
    void pause();
    Bank::Sound sound;
    Bank::File& file;

private:

    void playSound();

    //create a vorbis file in memory to later play the sound
    void createVorbisFile();

    void SubmitData(Ogg_stream_state& state, bool flush);

    sf::Sound soundPlayer;
    sf::SoundBuffer buffer;

    std::thread PlayingThread;
    std::vector<char> fileInMemory;
    //debug remove
    //std::ofstream fileOut;
};

