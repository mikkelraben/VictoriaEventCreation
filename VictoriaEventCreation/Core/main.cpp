#pragma once
#include "pch.h"

//if not building library launch an application
#include "Application.h"

#if defined(_WIN32) || defined(_WIN64)
#pragma warning(disable:28251)
#pragma warning(disable:4005)
#pragma warning(disable:4100)
#include <Windows.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, INT nCmdShow)
{
#pragma warning(default:28251)
#pragma warning(default:4005)
#pragma warning(default:4100)
#else
int main()
{
#endif
    try
    {
        BaseApp::Application();
    }
    catch (...)
    {
        RE_LogError("Unkown Error: Exception not catched terminating program");
    }
    Log::SaveLogs();

    return 0;
}



