#pragma once

#include <functional>
#include <map>
#include <string>
#include <vector>

namespace Nebula::Platform
{
    enum class FileType
    {
        NebulaScene
    };

    struct FileTypeData
    {
        std::string OpenButtonText;
        std::string SaveButtonText;

        std::string DefaultExtension;

        std::map<std::string, std::vector<std::string>> fileTypes;
    };

    class FileDialogs
    {
    public:
        static void OpenFile(FileType fileType, std::function<void(std::string)> callback);
        static void OpenFile_MT(FileType fileType, std::function<void(std::string)> callback);
        static void SaveFile(FileType fileType, std::function<void(std::string)> callback);
        static void SaveFile_MT(FileType fileType, std::function<void(std::string)> callback);

        static FileTypeData GetFileTypeData(FileType fileType)
        {
            switch (fileType)
            {
                case FileType::NebulaScene:
                    {
                        std::map<std::string, std::vector<std::string>> map;
                        map["Nebula Scene (*.nbscene)"] = { ".nbscene" };

                        return {
                            .OpenButtonText = "Open Scene",
                            .SaveButtonText = "Save Scene",
                            .DefaultExtension = ".nbscene",
                            .fileTypes = map
                        };
                    }
            }
        }
    };

}
