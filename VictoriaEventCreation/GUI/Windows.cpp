#include "pch.h"
#include "../Core/pch.h"
#include "Windows.h"
#include "../Sound/BankLoad.h"
#include "Widgets.h"

void BufferTest::Run()
{
    auto buffer = new unsigned char[1024];
    
    ogg_packet packet;

    packet.packet = buffer;
    packet.bytes = 1024;
    ogg_packet_clear(&packet);
}

void EventTool::Run()
{
    static int selected = 0;
    static std::string preview = "Pick A Sound";
    if (ImGui::BeginCombo("Sounds",preview.c_str(), 0))
    {
        std::scoped_lock lock(soundsFile.soundsMutex);
        for (size_t i = 0; i < soundsFile.sounds.size(); i++)
        {
            bool isSelected = selected == i;
            if (ImGui::Selectable(soundsFile.sounds[i].name.c_str()))
            {
                preview = soundsFile.sounds[i].name;
                selected = i;
            }

            if (isSelected)
            {
                ImGui::SetItemDefaultFocus();
                
            }
        }
        ImGui::EndCombo();
    }
    if(VecGui::Button("Play"))
    {
        sound = std::make_unique<Sound>(soundsFile.sounds[selected], soundsFile);
        sound->play();
    }

}

void EventTool::Opened()
{
    std::jthread thread(&Bank::File::ParseBankFile,&soundsFile);
    thread.detach();
}

void Console::Run()
{
    ImGuiListClipper clipper;
    clipper.Begin((int)Log::logs.size());
    while (clipper.Step())
    {
        for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
        {
            ImGui::TextUnformatted((Log::logs[i].type + Log::logs[i].log).c_str());
        }
    }
}