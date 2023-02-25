#pragma once
#include "../Core/Node.h"
#include "Trigger.h"

class Event
{
public:
    Event();
    std::vector<BasicNode*> children;
    std::vector<Trigger*> behaviourNodes;
};

