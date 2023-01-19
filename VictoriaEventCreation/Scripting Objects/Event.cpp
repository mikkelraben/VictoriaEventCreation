#include "pch.h"
#include "Event.h"

Event::Event()
{
    children.emplace_back(new Param<std::string>("", "Title"));
    children.emplace_back(new Param<std::string>("", "Description"));
    children.emplace_back(new Param<std::string>("", "Flavor Text"));
}
