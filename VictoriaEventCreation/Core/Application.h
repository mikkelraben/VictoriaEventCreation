#pragma once
#include "imgui/imgui_impl_glfw.h"

namespace BaseApp {
    static void glfw_error_callback(int error, const char* description);
    class Window;

    class Application
    {
    public:
        Application();

        void Run();

        //called every tick after other windows
        void (*Tick)() = nullptr;

        std::vector<Window*> windows;
        bool isDemoOpen = false;

    private:
        void InitWindow();
        void DrawGui();
    };

    class Window
    {
    public:
        Window(std::vector<Window*>& applicationWindows);
        virtual ~Window() = default;

        //call every frame
        virtual void Execute();

        //a vector of windows from application class
        std::vector<Window*>& windows;

        //should be set to a non empty value
        std::string name = "";

        //set to false if window should not be able to close
        bool canWindowClose = true;

        //set to true to open window and false to close window
        bool isWindowOpen = false;
    protected:

        //override to use standard window behaviour
        //called every frame no need to create window
        virtual void Run() {};

        //called when window has just opened
        virtual void Opened() {};

        //called when window has been closed, aka. next frame the window won't be displayed
        virtual void Exit() {};

        //called when there are unsaved changes and trying to close the window
        virtual void UnsavedChangesBlocking() {};

        bool unsavedChanges = false;
        bool windowHasOpened = false;

        //set to change window flags in ImGui::Begin
        ImGuiWindowFlags windowFlags = 0;
    };

}

