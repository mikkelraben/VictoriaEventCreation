#include "pch.h"
#include "Event.h"

Event::Event()
{
    children.emplace_back(new Param<std::string>("Insert Title", "Title"));
    children.emplace_back(new Param<std::string>("", "Description"));
    children.emplace_back(new Param<std::string>("", "Flavor Text"));
    children.emplace_back(new Param<Sound::Event>(Sound::Event("event:/SFX/UI/Alerts/event_appear"), "On Created",true,false));
    children.emplace_back(new Param<Sound::Event>(Sound::Event(""), "On Opened",false,true));
}