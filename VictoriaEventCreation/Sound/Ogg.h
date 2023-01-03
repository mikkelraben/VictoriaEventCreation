#pragma once
#include <ogg/ogg.h>
#include <vorbis/codec.h>

class Ogg_Packet
{
public:
    ogg_packet packet;
    Ogg_Packet()
    {
        packet.bytes = 0;
        packet.b_o_s = 0;
        packet.e_o_s = 0;
        packet.granulepos = 0;
        packet.packet = nullptr;
        packet.packetno = 0;
    }
    ~Ogg_Packet()
    {
        ogg_packet_clear(&packet);
    }
    operator ogg_packet() const { return packet; }
    ogg_packet* operator &() { return &packet; }

    ogg_packet* operator->() 
    {
        return &packet;
    }
};

class Ogg_stream_state
{
private:
    ogg_stream_state state;
public:
    Ogg_stream_state()
    {
        ogg_stream_init(&state, 1);
    }
    ~Ogg_stream_state()
    {
        ogg_stream_clear(&state);
    }
    operator ogg_stream_state() const { return state; }
    ogg_stream_state* operator &() { return &state; }

};

class Ogg_Page
{
private:
    ogg_page page;
public:
    Ogg_Page()
    {
        page.body = nullptr;
        page.body_len = 0;
        page.header = nullptr;
        page.header_len = 0;
    }
    //~Ogg_Page()
    //{
    //    delete[] page.body;
    //}

    operator ogg_page() const { return page; }
    ogg_page* operator &() { return &page; }

    ogg_page* operator->()
    {
        return &page;
    }

};


class Vorbis_info
{
public:
    vorbis_info vorbisComment;
    Vorbis_info()
    {
        vorbis_info_init(&vorbisComment);
    }
    ~Vorbis_info()
    {
        vorbis_info_clear(&vorbisComment);
    }
    operator vorbis_info() const { return vorbisComment; }
    vorbis_info* operator &() { return &vorbisComment; }
};

class Vorbis_comment
{
public:
    vorbis_comment vorbisComment;
    Vorbis_comment()
    {
        vorbis_comment_init(&vorbisComment);
    }
    ~Vorbis_comment()
    {
        vorbis_comment_clear(&vorbisComment);
    }
    operator vorbis_comment() const { return vorbisComment; }
    vorbis_comment* operator &() { return &vorbisComment; }
};
