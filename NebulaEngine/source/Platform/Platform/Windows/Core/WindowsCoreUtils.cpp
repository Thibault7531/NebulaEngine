#include "Platform/Nebula/CoreSystems/Core/CoreUtils.h"

#include <Windows.h>
#include <winrt/base.h>
#include <WindowsAppSDK-VersionInfo.h>

#include "Platform/Nebula/CoreSystems/Core/Log.h"

typedef HRESULT(WINAPI* PFN_MddBootstrapInitialize)(UINT32, PCWSTR, PCWSTR);

namespace Nebula::Platform
{

    void CoreUtils::InitThread()
    {
        LoadLibrary("Microsoft.WindowsAppRuntime.Bootstrap.dll");
        const auto pfn_mdd_bootstrap_initialize = reinterpret_cast<PFN_MddBootstrapInitialize>(GetProcAddress(GetModuleHandle("Microsoft.WindowsAppRuntime.Bootstrap.dll"), "MddBootstrapInitialize"));

        HRESULT hr = pfn_mdd_bootstrap_initialize(Microsoft::WindowsAppSDK::Release::MajorMinor, Microsoft::WindowsAppSDK::Release::VersionTag, PCWSTR(Microsoft::WindowsAppSDK::Runtime::Version::UInt64));
        if (FAILED(hr)) NB_PLATFORM_ERROR("Failed loading Windows App SDK with error code {}", hr);

        winrt::init_apartment();

        SetProcessDPIAware();
    }

    void CoreUtils::InitThread_MT()
    {
        winrt::init_apartment(winrt::apartment_type::single_threaded);
    }

    void CoreUtils::PumpEvents()
    {
        MSG msg;
        if (PeekMessage(&msg, nullptr, 0, 0, 0))
        {
            GetMessage(&msg, nullptr, 0, 0);
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
}
