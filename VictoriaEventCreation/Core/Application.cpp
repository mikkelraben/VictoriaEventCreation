#include "pch.h"
#include "Application.h"
#include "freeImage/FreeImage.h"
#include "Graphics.h"
#include "Windows.h"
#include "../GUI/Windows.h"
#include <GLFW/glfw3.h>

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
        InitWindow();
        Window* eventThingy = new EventTool(windows);
        windows.push_back(eventThingy);

        Window* console = new Console(windows);
        windows.push_back(console);


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
        }
    }

    Window::Window(std::vector<Window*>& applicationWindows) : windows(applicationWindows)
    {

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

        ImGui::Begin(name.c_str(), &isWindowOpen, windowFlags);

        //if window should close
        if (!isWindowOpen)
        {
            if (!unsavedChanges)
            {
                Exit();
                windowHasOpened = true;
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
}

