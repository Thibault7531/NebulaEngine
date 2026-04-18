#include <thread>

#include "Platform/Nebula/Dialogs/FileDialogs.h"

#include "SwiftFileDialogs.h"

namespace Nebula::Platform
{
    void FileDialogs::OpenFile(FileType fileType, std::function<void(std::string)> callback)
    {
        SwiftFileDialogs::OpenFile(callback);
    }

    void FileDialogs::OpenFile_MT(FileType fileType, std::function<void(std::string)> callback)
    {
    }

    void FileDialogs::SaveFile(FileType fileType, std::function<void(std::string)> callback)
    {
        SwiftFileDialogs::SaveFile(fileType, callback);
    }

    void FileDialogs::SaveFile_MT(FileType fileType, std::function<void(std::string)> callback)
    {
    }
}
