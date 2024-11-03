workspace "Nebula"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

	platforms
	{
		"WindowsX86",
		"WindowsARM",
		"MacOS",
		"LinuxX86",
		"LinuxARM",
		"IOS",
		"Android"
	}

	filter "platforms:WindowsX86"
		system "windows"
		architecture "x64"
		
	filter "platforms:WindowsARM"
		system "windows"
		architecture "ARM64"

	filter "platforms:MacOS"
		system "macosx"
		architecture "universal"

	filter "platforms:LinuxX86"
		system "linux"
		architecture "x64"

	filter "platforms:LinuxARM"
		system "linux"
		architecture "ARM64"

	filter "platforms:IOS"
		system "ios"
		architecture "universal"

	filter "platforms:Android"
		system "android"
		architecture "ARM64"

	filter "action:vs2022"
		os.execute("cmake -S \"NebulaEngine/vendor/SDL3\" -B \"bin-int/SDL3\"")
		os.execute("msbuild \"bin-int/SDL3/SDL3.sln\" /property:Configuration=Debug")
		os.execute("msbuild \"bin-int/SDL3/SDL3.sln\" /property:Configuration=Release")

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

include "NebulaEngine/vendor/Glad"
include "NebulaEngine/vendor/imgui"

project "NebulaEngine"
	location "NebulaEngine"
	kind "SharedLib"
	language "C++"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "nbpch.h"
	pchsource "NebulaEngine/source/nbpch.cpp"

	files
	{
		"%{prj.name}/source/**.h",
		"%{prj.name}/source/**.cpp"
	}

	includedirs
	{
		"%{prj.name}/vendor/spdlog/include",
		"%{prj.name}/vendor/SDL3/include/SDL3",
		"%{prj.name}/vendor/SDL3/include",
		"%{prj.name}/source",
		"%{prj.name}/vendor/Glad/include",
		"%{prj.name}/vendor/imgui"
	}

	links
	{
		"Glad",
		"ImGui"
	}

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "latest"

		defines
		{
			"NB_PLATFORM_WINDOWS",
			"NB_BUILD_DLL"
		}

		postbuildcommands
		{
			("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/TestGame")
		}

	filter "configurations:Debug"
		defines "NB_DEBUG"
		buildoptions
		{
			"/MDd",
			"/utf-8"
		}
		symbols "On"

		defines
		{
			"NB_ENABLE_ASSERTS"
		}

		links
		{
			"bin-int/SDL3/Debug/SDL3"
		}

		postbuildcommands
		{
			("{COPYFILE} %{wks.location}bin-int/SDL3/Debug/SDL3.dll %{wks.location}bin/" .. outputdir .. "/TestGame")
		}

	filter "configurations:Release"
		defines "NB_RELEASE"
		buildoptions
		{
			"/MD",
			"/utf-8"
		}
		optimize "On"

		links
		{
			"bin-int/SDL3/Release/SDL3"
		}

		postbuildcommands
		{
			("{COPYFILE} %{wks.location}bin-int/SDL3/Release/SDL3.dll %{wks.location}bin/" .. outputdir .. "/TestGame")
		}

	filter "configurations:Dist"
		defines "NB_DIST"
		buildoptions
		{
			"/MD",
			"/utf-8"
		}
		optimize "On"

		links
		{
			"bin-int/SDL3/Release/SDL3"
		}

		postbuildcommands
		{
			("{COPYFILE} %{wks.location}bin-int/SDL3/Release/SDL3.dll %{wks.location}bin/" .. outputdir .. "/TestGame")
		}


project "TestGame"
	location "TestGame"
	kind "ConsoleApp"
	language "C++"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/source/**.h",
		"%{prj.name}/source/**.cpp"
	}

	includedirs
	{
		"NebulaEngine/vendor/spdlog/include",
		"NebulaEngine/source",
		"NebulaEngine/vendor/SDL3/include",
		"NebulaEngine/vendor/SDL3/include/SDL3",
		"NebulaEngine/vendor/imgui"
	}

	links
	{
		"NebulaEngine"
	}

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "latest"

		defines
		{
			"NB_PLATFORM_WINDOWS",
		}

	filter "configurations:Debug"
		defines "NB_DEBUG"
		buildoptions
		{
			"/MDd",
			"/utf-8"
		}
		symbols "On"

	filter "configurations:Release"
		defines "NB_RELEASE"
		buildoptions
		{
			"/MD",
			"/utf-8"
		}
		optimize "On"

	filter "configurations:Dist"
		defines "NB_DIST"
		buildoptions
		{
			"/MD",
			"/utf-8"
		}
		optimize "On"