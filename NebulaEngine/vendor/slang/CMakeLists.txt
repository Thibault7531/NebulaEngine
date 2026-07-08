include(FetchContent)

if (nebula_cmake_platform STREQUAL Windows)
    set(SLANG_URL "https://github.com/shader-slang/slang/releases/download/v2026.12.2/slang-2026.12.2-windows-x86_64.zip")
    set(SLANG_HASH "SHA256=e44a29e4ba766e892db19e7f491b0c1fc21f548a0380a1b2931039569bf747e7")
elseif (nebula_cmake_platform STREQUAL MacOS)
    set(SLANG_URL "https://github.com/shader-slang/slang/releases/download/v2026.12.2/slang-2026.12.2-macos-x86_64.zip")
    set(SLANG_HASH "SHA256=6e5ac21da34ba4a8848d31714130605103456f634223f1cc752f194d16a00a3a")
elseif (nebula_cmake_platform STREQUAL Linux)
    set(SLANG_URL "https://github.com/shader-slang/slang/releases/download/v2026.12.2/slang-2026.12.2-linux-x86_64.tar.gz")
    set(SLANG_HASH "SHA256=5533415953112ddeb0a935755bdd2da5de530e6528a560a32ad809c9d9faf29c")
endif ()

FetchContent_Declare(
        slang_prebuilt
        URL ${SLANG_URL}
        URL_HASH ${SLANG_HASH}
        DOWNLOAD_EXTRACT_TIMESTAMP NEW
)

FetchContent_MakeAvailable(slang_prebuilt)

if (NOT TARGET slang::slang)
    add_library(slang::slang INTERFACE IMPORTED GLOBAL)

    target_include_directories(slang::slang INTERFACE ${slang_prebuilt_SOURCE_DIR}/include)

    if (nebula_cmake_platform STREQUAL Windows)
        target_link_libraries(slang::slang INTERFACE ${slang_prebuilt_SOURCE_DIR}/lib/slang.lib)
    elseif (nebula_cmake_platform STREQUAL MacOS)
        target_link_libraries(slang::slang INTERFACE ${slang_prebuilt_SOURCE_DIR}/lib/libslang.dylib)
    elseif (nebula_cmake_platform STREQUAL Linux)
        target_link_libraries(slang::slang INTERFACE ${slang_prebuilt_SOURCE_DIR}/lib/libslang.so)
    endif()
endif ()