#pragma once
#include "../Core/Application.h"
#include "../Sound/BankLoad.h"
#include "../Scripting Objects/Event.h"

class EventTool : public BaseApp::Window
{
public:
    explicit EventTool(std::vector<Window*>& applicationWindows) : Window(applicationWindows)
    {
        name = "Event Tool";
        isWindowOpen = true;
    }
    void Run() override;
    void Properties() override;
private:
    Sound::SoundSystem soundSystem;
    Event object;
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

class Properties : public BaseApp::Window
{
public:
    explicit Properties(std::vector<Window*>& applicationWindows) : Window(applicationWindows)
    {
        name = "Properties";
        isWindowOpen = true;
    }
    void Run() override;
};

