# CMake file for TestGame target


# Setup Target
add_executable(TestGame source/TestGame.cpp)


# Output Directory
set_target_properties(TestGame PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}-${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}/TestGame")


# Defines
target_compile_definitions(TestGame PUBLIC NB_PLATFORM_WINDOWS)
target_compile_definitions(TestGame PRIVATE "IMGUI_API=__declspec(dllimport)")


# Libraries
target_link_libraries(TestGame PUBLIC Nebula)
target_include_directories(TestGame PRIVATE ../NebulaEngine/source ../NebulaEngine/vendor/spdlog/include ../NebulaEngine/vendor/SDL3/include/SDL3 ../NebulaEngine/vendor/SDL3/include ../NebulaEngine/vendor/imgui)
