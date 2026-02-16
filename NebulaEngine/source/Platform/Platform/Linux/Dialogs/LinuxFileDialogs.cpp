#include <thread>
#include <utility>
#include <SDL3/SDL_dialog.h>

#include "Core/Nebula/CoreSystems/Core/Log.h"
#include "Platform/Nebula/CoreSystems/Windows/WindowManager.h"
#include "Platform/Nebula/Dialogs/FileDialogs.h"

namespace Nebula::Platform
{
    struct callback_data
    {
        std::function<void(std::string)>* callback;
        bool called = false;
    };

    SDL_DialogFileFilter GetLinuxFileDialogFilter(FileType fileType)
    {
        switch (fileType)
        {
        case FileType::NebulaScene:
            {
                return {"Nebula Scene (*.nbscene)", "nbscene"};
            }
        }
        return {};
    }

    void FileDialogs::OpenFile(FileType fileType, std::function<void(std::string)> callback)
    {
        std::thread thread{OpenFile_MT, fileType, callback};
        thread.detach();
    }

    void FileDialogs::OpenFile_MT(FileType fileType, std::function<void(std::string)> callback)
    {
        callback_data data{.callback = &callback};

        FileTypeData fileTypeData = GetFileTypeData(fileType);
        std::vector<SDL_DialogFileFilter> filters;
        for (auto type : fileTypeData.fileTypes)
        {
            filters.push_back(GetLinuxFileDialogFilter(fileType));
        }

        SDL_ShowOpenFileDialog([](void* data, const char* const *filelist, int filter)
        {
            auto* callbackData = static_cast<callback_data*>(data);
            auto callback = *callbackData->callback;
            if (*filelist != nullptr) callback(filelist[0]);
            callbackData->called = true;
        }, &data, static_cast<SDL_Window*>(WindowManager::Get().GetMainWindow()->GetHandle()), filters.data(), filters.size(), nullptr, false);

        while (!data.called) {}
    }

    void FileDialogs::SaveFile(FileType fileType, std::function<void(std::string)> callback)
    {
        std::thread thread{SaveFile_MT, fileType, callback};
        thread.detach();
    }

    void FileDialogs::SaveFile_MT(FileType fileType, std::function<void(std::string)> callback)
    {
        callback_data data{.callback = &callback};

        FileTypeData fileTypeData = GetFileTypeData(fileType);
        std::vector<SDL_DialogFileFilter> filters;
        for (auto type : fileTypeData.fileTypes)
        {
            filters.push_back(GetLinuxFileDialogFilter(fileType));
        }

        SDL_ShowSaveFileDialog([](void* data, const char* const *filelist, int filter)
        {
            auto* callbackData = static_cast<callback_data*>(data);
            auto callback = *callbackData->callback;
            if (*filelist != nullptr) callback(filelist[0]);
            callbackData->called = true;
        }, &data, static_cast<SDL_Window*>(WindowManager::Get().GetMainWindow()->GetHandle()), filters.data(), filters.size(), nullptr);

        while (!data.called) {}
    }
}
