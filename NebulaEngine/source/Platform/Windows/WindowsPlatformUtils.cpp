#include <commdlg.h>
#include <ranges>
#include <SDL.h>

#include "Nebula/CoreSystems/Application/Application.h"
#include "Nebula/CoreSystems/Windows/WindowManager.h"
#include "Nebula/Utils/PlatformUtils.h"

namespace Nebula
{

    std::string FileDialogs::OpenFile(const std::vector<FileType>& fileTypes)
    {
        // TODO: Update to common item dialog
        // TODO: Run in seperate thread
        char filter[260] = { 0 };
        ConvertFilter(fileTypes, filter);
        SDL_PropertiesID props = SDL_GetWindowProperties(static_cast<SDL_Window*>(WindowManager::Get().GetMainWindow()->GetNativeWindow()));
        const auto hwnd = static_cast<HWND>(SDL_GetPointerProperty(props, SDL_PROP_WINDOW_WIN32_HWND_POINTER, nullptr));
        OPENFILENAMEA ofn;
        CHAR szFile[260] = { 0 };

        ZeroMemory(&ofn, sizeof(OPENFILENAMEA));
        ofn.lStructSize = sizeof(OPENFILENAMEA);
        ofn.hwndOwner = hwnd;
        ofn.lpstrFile = szFile;
        ofn.nMaxFile = sizeof(szFile);
        ofn.lpstrFilter = filter;
        ofn.nFilterIndex = 1;
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
        if (GetOpenFileNameA(&ofn) == TRUE)
        {
            return ofn.lpstrFile;
        }
        return {};
    }

    std::string FileDialogs::SaveFile(const char* filter)
    {
        // TODO: Update to common item dialog
        // TODO: Run in seperate thread
        SDL_PropertiesID props = SDL_GetWindowProperties(static_cast<SDL_Window*>(WindowManager::Get().GetMainWindow()->GetNativeWindow()));
        const auto hwnd = static_cast<HWND>(SDL_GetPointerProperty(props, SDL_PROP_WINDOW_WIN32_HWND_POINTER, nullptr));
        std::string extension = filter + strlen(filter) + 1;
        std::string defExt = extension.substr(2, extension.find_first_of(';'));
        OPENFILENAMEA ofn;
        CHAR szFile[260] = { 0 };

        ZeroMemory(&ofn, sizeof(OPENFILENAMEA));
        ofn.lStructSize = sizeof(OPENFILENAMEA);
        ofn.hwndOwner = hwnd;
        ofn.lpstrFile = szFile;
        ofn.nMaxFile = sizeof(szFile);
        ofn.lpstrFilter = filter;
        ofn.nFilterIndex = 1;
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
        ofn.lpstrDefExt = defExt.c_str();
        if (GetSaveFileNameA(&ofn) == TRUE)
        {
            return ofn.lpstrFile;
        }
        return {};
    }

    void FileDialogs::ConvertFilter(const std::vector<FileType>& fileTypes, char* filter)
    {
        int filterLength = 0;
        for (auto filetype : fileTypes)
        {
            switch (filetype)
            {
            case FileType::NebulaScene:
                memcpy_s(filter + filterLength, 36, "Nebula Scene (*.nbscene)\0*.nbscene\0", 36);
            }
        }
    }

}
