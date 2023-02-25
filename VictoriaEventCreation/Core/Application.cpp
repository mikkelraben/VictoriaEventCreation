#include "pch.h"
#include "Application.h"
#include "freeImage/FreeImage.h"
#include "Graphics.h"
#include "Windows.h"
#include "../GUI/Windows.h"
#include <GLFW/glfw3.h>
#include "../Sound/BankLoad.h"
#include "../Scripting Objects/Trigger.h"

namespace BaseApp {
    static void glfw_error_callback(int error, const char* description)
    {
        fprintf(stderr, "Glfw Error %d: %s\n", error, description);
    }

    void FreeImageErrorHandler(FREE_IMAGE_FORMAT fif, const char* e)
    {
        std::string format = "Unknown";
        if (fif != FIF_UNKNOWN)
        {
            format = FreeImage_GetFormatFromFIF(fif);
        }
        RE_LogError("Could not load file with " + format + " format. Error From FreeImage: " + e);
    }

    Application::Application()
    {
        Settings::LoadSettings();
        Sound::SoundSystem::InitSoundSystem();
        Scripting::InitScripting();
        InitWindow();

        Window* console = new Console(windows);
        windows.push_back(console);

        Window* eventThingy = new EventTool(windows);
        windows.push_back(eventThingy);



        SettingsEditor* settings = new SettingsEditor(windows);
        windows.push_back(settings);

        Properties* properties = new Properties(windows);
        windows.push_back(properties);

        
        Run();
    }

    void Application::InitWindow()
    {
        FreeImage_SetOutputMessage(FreeImageErrorHandler);
        std::string FIVersion = FreeImage_GetVersion();
        RE_LogMessage("FreeImage Version: " + FIVersion);

        std::string glfwVersion = glfwGetVersionString();
        RE_LogMessage("glfw Version: " + glfwVersion);


        RE_LogMessage("Initializing GLFW");
        glfwSetErrorCallback(glfw_error_callback);
        if (!glfwInit()) {
            RE_LogMessage("Failed to initialize GLFW!");
            return;
        }

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        RE_LogMessage("Initialized GLFW! Creating window");
        

#ifdef _DEBUG
        isDemoOpen = true;
#endif // DEBUG
    }

    void Application::DrawGui()
    {
        if (isDemoOpen) {
            ImGui::ShowDemoWindow(&isDemoOpen);
        }

        //raw due to windows could destroy themselves
        for (size_t i = 0; i < windows.size(); i++)
        {
            windows[i]->Execute();
        }

        for (size_t i = 0; i < windows.size(); i++)
        {
            if (windows[i]->requestDelete)
            {
                windows.erase(windows.begin() + i);
            }
        }
    }

    void Application::Run()
    {
        Graphics graphics = Graphics();

        while (!glfwWindowShouldClose(graphics.window))
        {
            // Poll and handle events (inputs, window resize, etc.)
                // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
                // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
                // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
                // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
            glfwPollEvents();
            
            // if swapchain needs to be recreated
            graphics.SwapchainRebuild();

            // Start the Dear ImGui frame
            ImGui_ImplVulkan_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
            
            ImGui::DockSpaceOverViewport();

            DrawGui();

            if (Tick)
            {
                Tick();
            }

            ImGui::Render();
            graphics.RenderFrame();
            Sound::SoundSystem::Update();
        }

        Sound::SoundSystem::DeleteSoundSystem();
    }

    Window::Window(std::vector<Window*>& applicationWindows) : windows(applicationWindows)
    {
        static int counter = 0;
        counter++;
        windowID = counter;
    }

    void Window::Execute()
    {
        if (!isWindowOpen && !unsavedChanges)
        {
            return;
        }
        if (!windowHasOpened)
        {
            Opened();
            windowHasOpened = true;
        }

        bool* closeWindow = &isWindowOpen;
        if (!canWindowClose)
        {
            closeWindow = nullptr;
        }

        if (unsavedChanges)
        {
            windowFlags |= ImGuiWindowFlags_UnsavedDocument;
        }
        else
        {
            windowFlags &= ~ImGuiWindowFlags_UnsavedDocument;
        }

        ImGui::Begin(name.c_str(), closeWindow, windowFlags);
        //updated isSelected
        isSelected = ImGui::IsWindowFocused(ImGuiHoveredFlags_ChildWindows);

        //if window should close
        if (!isWindowOpen)
        {
            if (!unsavedChanges)
            {
                Exit();
                windowHasOpened = false;
                isSelected = false;
                ImGui::End();
                requestDelete = true;
                return;
            }
            else
            {
                UnsavedChangesBlocking();
            }
        }

        //call virtual function
        Run();

        ImGui::End();


    }

    void Window::Properties()
    {
        ImGui::Text("No Properties");
    }

    int Window::getId()
    {
        return windowID;
    }

    void Window::UnsavedChangesBlocking()
    {
        ImGui::OpenPopup("Save Settings?");
        ImGui::SetNextWindowSizeConstraints({ 32,128 }, { -1,-1 });
        if (ImGui::BeginPopupModal("Save Settings?", 0, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar))
        {
            ImGui::Text("Some Settings Are Not Saved, \nDo You Want To Continue Or Go Back");
            ImGui::Separator();
            if (ImGui::Button("Go Back", { 120,0 }))
            {
                isWindowOpen = true;
                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine();
            if (ImGui::Button("Continue", { 120,0 }))
            {
                unsavedChanges = false;
            }
            ImGui::EndPopup();
        }
    }
}

