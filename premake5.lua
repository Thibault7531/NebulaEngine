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

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

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
		"%{prj.name}/source"
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
		symbols "On"

	filter "configurations:Release"
		defines "NB_RELEASE"
		optimize "On"

	filter "configurations:Dist"
		defines "NB_DIST"
		optimize "On"


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
		"NebulaEngine/source"
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
		symbols "On"

	filter "configurations:Release"
		defines "NB_RELEASE"
		optimize "On"

	filter "configurations:Dist"
		defines "NB_DIST"
		optimize "On"