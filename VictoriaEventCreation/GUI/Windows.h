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
    Bank::File soundsFile = Bank::File(Settings::gameDirectory.getSetting() / "game\\sound\\banks" / "Events.bank");
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

class SettingsEditor : public BaseApp::Window
{
public:
    explicit SettingsEditor(std::vector<Window*>& applicationWindows) : Window(applicationWindows)
    {
        name = "Settings";
        isWindowOpen = true;
    }
    void Run() override;
};
