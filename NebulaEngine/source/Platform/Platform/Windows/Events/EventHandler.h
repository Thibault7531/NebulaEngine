#pragma once

#include <stdint.h>
#include <Windows.h>

#include "Platform/Nebula/CoreSystems/Windows/Window.h"

namespace Nebula::Platform
{

    class EventHandler
    {
    public:
        static LRESULT SubclassProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);

    public:
        static int32_t ConvertHitTestResult(HitTest hittest);
    };

}
