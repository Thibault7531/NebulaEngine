# Finds the latest installed version of the Windows SDK

set(ENV{WINDOWS_SDK_FOUND} OFF)

if (nebula_cmake_platform STREQUAL Windows)
    execute_process(
            COMMAND reg query "HKLM\\SOFTWARE\\Microsoft\\Windows Kits\\Installed Roots" /v "KitsRoot10"
            OUTPUT_VARIABLE _kitsroot10_output
            ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    if (_kitsroot10_output MATCHES "KitsRoot10[ \\t]+REG_SZ[ \\t]+(.+)")
        set(_sdk_root "${CMAKE_MATCH_1}")

        file(GLOB _sdk_versions RELATIVE "${_sdk_root}/Include" "${_sdk_root}/Include/10.*")
        if (_sdk_versions)
            list(SORT _sdk_versions COMPARE NATURAL ORDER DESCENDING)
            list(GET _sdk_versions 0 WINDOWS_SDK_VERSION)
            set(ENV{WINDOWS_SDK_VERSION} ${WINDOWS_SDK_VERSION})

            set(ENV{WINDOWS_SDK_INCLUDE_DIR} "${_sdk_root}\\Include\\$ENV{WINDOWS_SDK_VERSION}")
            set(ENV{WINDOWS_SDK_LIB_DIR} "${_sdk_root}\\Lib\\$ENV{WINDOWS_SDK_VERSION}")
            set(ENV{WINDOWS_SDK_BIN_DIR} "${_sdk_root}\\bin\\$ENV{WINDOWS_SDK_VERSION}")
            set(ENV{WINDOWS_SDK_METADATA_DIR} "${_sdk_root}\\UnionMetadata\\$ENV{WINDOWS_SDK_VERSION}")

            set(ENV{WINDOWS_SDK_FOUND} ON)

        endif ()

    endif ()

    if ($ENV{WINDOWS_SDK_FOUND})
        message(STATUS "Found Windows SDK $ENV{WINDOWS_SDK_VERSION}")
        message(STATUS "Include dir: $ENV{WINDOWS_SDK_INCLUDE_DIR}")
        message(STATUS "Lib dir: $ENV{WINDOWS_SDK_LIB_DIR}")
        message(STATUS "Bin dir: $ENV{WINDOWS_SDK_BIN_DIR}")
        message(STATUS "Metadata dir: $ENV{WINDOWS_SDK_METADATA_DIR}")
    else ()
        message(WARNING "Could not find Windows SDK")
    endif ()

    add_custom_command(
            OUTPUT "${CMAKE_BINARY_DIR}/NebulaEngine/vendor/winrt/winmd/Microsoft.UI.winmd"
            COMMAND powershell -ExecutionPolicy Bypass -File ${CMAKE_SOURCE_DIR}/Scripts/copy-winmd.ps1
            DEPENDS ${CMAKE_SOURCE_DIR}/Scripts/copy-winmd.ps1
            COMMENT "Copying Windows App SDK .winmd files"
            VERBATIM
            BYPRODUCTS
    )

    add_custom_target(copy_winmd DEPENDS "${CMAKE_BINARY_DIR}/NebulaEngine/vendor/winrt/winmd/Microsoft.UI.winmd")

    #add_custom_target(copy_winmd
    #                  COMMAND powershell -ExecutionPolicy Bypass -File ${CMAKE_SOURCE_DIR}/Scripts/copy-winmd.ps1
    #)

    set(ENV{GENERATED_DIR} "${CMAKE_BINARY_DIR}/cppwinrt")
    set(CPPWINRT_EXE "$ENV{WINDOWS_SDK_BIN_DIR}/x64/cppwinrt.exe")

    add_custom_command(
            OUTPUT $ENV{GENERATED_DIR}/stamp
            COMMAND "${CPPWINRT_EXE}" -verbose
            -input "$ENV{WINDOWS_SDK_METADATA_DIR}/Windows.winmd"
            -input "${CMAKE_BINARY_DIR}/NebulaEngine/vendor/winrt/winmd/Microsoft.UI.winmd"
            -input "${CMAKE_BINARY_DIR}/NebulaEngine/vendor/winrt/winmd/Microsoft.Graphics.winmd"
            -input "${CMAKE_BINARY_DIR}/NebulaEngine/vendor/winrt/winmd/Microsoft.Windows.ApplicationModel.WindowsAppRuntime.winmd"
            -input "${CMAKE_BINARY_DIR}/NebulaEngine/vendor/winrt/winmd/Microsoft.Windows.ApplicationModel.DynamicDependency.winmd"
            -input "${CMAKE_BINARY_DIR}/NebulaEngine/vendor/winrt/winmd/Microsoft.Windows.Storage.winmd"
            -input "${CMAKE_BINARY_DIR}/NebulaEngine/vendor/winrt/winmd/Microsoft.Windows.Storage.Pickers.winmd"
            -output "$ENV{GENERATED_DIR}"
            COMMAND ${CMAKE_COMMAND} -E touch "$ENV{GENERATED_DIR}/stamp"
            DEPENDS ${WINMD_INPUTS}
            COMMENT "Generating C++/WinRT headers using cppwinrt"
            COMMAND_EXPAND_LISTS
    )

    add_custom_target(generate_winrt ALL DEPENDS $ENV{GENERATED_DIR}/stamp)
    add_dependencies(generate_winrt copy_winmd)

endif ()