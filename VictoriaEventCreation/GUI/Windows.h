#pragma once
#include "../Core/Application.h"
#include "../Sound/Sound.h"

class BufferTest : public BaseApp::Window
{
public:
    explicit BufferTest(std::vector<Window*>& applicationWindows) : Window(applicationWindows)
    {
        name = "Buffer Test";
        isWindowOpen = true;
    }
    void Run() override;
};

class EventTool : public BaseApp::Window
{
public:
    explicit EventTool(std::vector<Window*>& applicationWindows) : Window(applicationWindows)
    {
        name = "Event Tool";
        isWindowOpen = true;
    }
    void Run() override;
    void Opened() override;
private:
    Bank::File soundsFile = Bank::File(std::filesystem::current_path() / "Events.bank");
    std::unique_ptr<Sound> sound;
    std::jthread soundLoadThread;
};

class Console : public BaseApp::Window
{
public:
    explicit Console(std::vector<Window*>& applicationWindows) : Window(applicationWindows)
    {
        name = "Console";
        isWindowOpen = true;
    }
    void Run() override;
};
