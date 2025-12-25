#include "Platform/Nebula/Dialogs/FileDialogs.h"

#include <Windows.h>
#include <winrt/Microsoft.UI.Windowing.h>
#include <winrt/Microsoft.Windows.Storage.Pickers.h>
#include <winrt/Windows.Foundation.Collections.h>

#include "Platform/Nebula/CoreSystems/Core/CoreUtils.h"
#include "Platform/Nebula/CoreSystems/Windows/WindowManager.h"

namespace Nebula::Platform
{
    void FileDialogs::OpenFile(FileType fileType, std::function<void(std::string)> callback)
    {
        std::thread thread{FileDialogs::OpenFile_MT, fileType, callback};
        thread.detach();
    }

    void FileDialogs::OpenFile_MT(FileType fileType, std::function<void(std::string)> callback)
    {
        FileTypeData fileTypeData = GetFileTypeData(fileType);

        CoreUtils::InitThread_MT();

        winrt::Microsoft::UI::Windowing::AppWindow* window = static_cast<winrt::Microsoft::UI::Windowing::AppWindow*>(WindowManager::Get().GetMainWindow()->GetNativeWindow());
        winrt::Microsoft::Windows::Storage::Pickers::FileOpenPicker picker{window->Id()};

        picker.CommitButtonText(winrt::to_hstring(fileTypeData.OpenButtonText));
        winrt::Windows::Foundation::Collections::IMap<winrt::hstring, winrt::Windows::Foundation::Collections::IVector<winrt::hstring>> fileTypes = picker.FileTypeChoices();
        for (auto fileType : fileTypeData.fileTypes)
        {
            winrt::Windows::Foundation::Collections::IVector<winrt::hstring> extensions = winrt::single_threaded_vector<winrt::hstring>();
            for (auto extension : fileType.second)
            {
                extensions.Append(winrt::to_hstring(extension));
            }
            fileTypes.Insert(winrt::to_hstring(fileType.first), extensions);
        }
        picker.ViewMode(winrt::Microsoft::Windows::Storage::Pickers::PickerViewMode::List);

        auto request = picker.PickSingleFileAsync();

        while (request.Status() == winrt::Windows::Foundation::AsyncStatus::Started)
        {
            CoreUtils::PumpEvents();
        }

        auto result = request.GetResults();
        if (!result) return;
        callback(winrt::to_string(result.Path()));
    }

    void FileDialogs::SaveFile(FileType fileType, std::function<void(std::string)> callback)
    {
        std::thread thread{FileDialogs::SaveFile_MT, fileType, callback};
        thread.detach();
    }

    void FileDialogs::SaveFile_MT(FileType fileType, std::function<void(std::string)> callback)
    {
        FileTypeData fileTypeData = GetFileTypeData(fileType);

        CoreUtils::InitThread_MT();

        winrt::Microsoft::UI::Windowing::AppWindow* window = static_cast<winrt::Microsoft::UI::Windowing::AppWindow*>(WindowManager::Get().GetMainWindow()->GetNativeWindow());
        winrt::Microsoft::Windows::Storage::Pickers::FileSavePicker picker{window->Id()};

        picker.CommitButtonText(winrt::to_hstring(fileTypeData.SaveButtonText));
        picker.DefaultFileExtension(winrt::to_hstring(fileTypeData.DefaultExtension));
        winrt::Windows::Foundation::Collections::IMap<winrt::hstring, winrt::Windows::Foundation::Collections::IVector<winrt::hstring>> fileTypes = picker.FileTypeChoices();
        for (auto fileType : fileTypeData.fileTypes)
        {
            winrt::Windows::Foundation::Collections::IVector<winrt::hstring> extensions = winrt::single_threaded_vector<winrt::hstring>();
            for (auto extension : fileType.second)
            {
                extensions.Append(winrt::to_hstring(extension));
            }
            fileTypes.Insert(winrt::to_hstring(fileType.first), extensions);
        }

        auto request = picker.PickSaveFileAsync();

        while (request.Status() == winrt::Windows::Foundation::AsyncStatus::Started)
        {
            MSG msg;
            if (PeekMessage(&msg, nullptr, 0, 0, 0))
            {
                GetMessage(&msg, nullptr, 0, 0);
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }

        auto result = request.GetResults();
        if (!result) return;
        callback(winrt::to_string(result.Path()));
    }
}
