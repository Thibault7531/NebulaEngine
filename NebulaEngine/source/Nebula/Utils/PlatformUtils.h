#pragma once

namespace Nebula
{

    class FileDialogs
    {
    public:
        enum class FileType
        {
            NebulaScene
        };

        static std::string OpenFile(const std::vector<FileType>& fileTypes);
        static std::string SaveFile(const char* filter);

    private:
        static void ConvertFilter(const std::vector<FileType>& fileTypes, char* filter);
    };

}
