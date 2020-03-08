workspace "GameEngineProject"
	architecture "x86_64"
	startproject "Sandbox"

	configurations
	{
		"Debug",
		"Release"
	}
	
	flags
	{
		"MultiProcessorCompile"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Include directories relative to root folder (solution directory)
IncludeDir = {}
IncludeDir["Glad"] = "Engine/dependencies/Glad/include"
IncludeDir["GLFW"] = "Engine/dependencies/GLFW/include"
IncludeDir["glm"] = "Engine/dependencies/glm"
IncludeDir["ImGui"] = "Engine/dependencies/imgui"
IncludeDir["stb_image"] = "Engine/dependencies/stb_image"
IncludeDir["assimp"] = "Engine/dependencies/assimp_prebuild/include"
IncludeDir["entityX"] = "Engine/dependencies/entityX"

group "Dependencies"
	include "Engine/dependencies/entityX"
	include "Engine/dependencies/Glad"
	include "Engine/dependencies/GLFW"
	include "Engine/dependencies/imgui"

group ""

project "Engine"
	location "Engine"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "pch.h"
	pchsource "Engine/src/pch.cpp"

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/dependencies/glm/glm/**.hpp",
		"%{prj.name}/dependencies/glm/glm/**.inl",
		"%{prj.name}/dependencies/stb_image/**.h",
		"%{prj.name}/dependencies/stb_image/**.cpp"
	}

	defines
	{
		"_CRT_SECURE_NO_WARNINGS"
	}

	includedirs
	{
		"%{prj.name}/src",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.ImGui}",
		"%{prj.name}/dependencies/spdlog/include",
		"%{IncludeDir.stb_image}",
		"%{IncludeDir.assimp}",
		"%{IncludeDir.entityX}"
	}

	links 
	{
		"Glad",
		"GLFW",
		"ImGui",
		"entityX",
		"opengl32.lib",
		"assimp-vc142-mt.lib"
	}

	filter "system:windows"
		systemversion "latest"

		defines
		{
			"GLFW_INCLUDE_NONE"
		}

	filter "configurations:Debug"
		defines "DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "RELEASE"
		runtime "Release"
		optimize "on"

project "Game"
	location "Game"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs
	{
		"Engine/src",
		"Engine/dependencies",
		"%{IncludeDir.glm}",
		"%{IncludeDir.entityX}",
		"Engine/dependencies/spdlog/include"
	}

	links
	{
		"Engine",
		"assimp-vc142-mt.lib"
	}

	postbuildcommands {
		"{COPY} assimp-vc142-mt.dll %{cfg.targetdir}"
	}

	filter "system:windows"
		systemversion "latest"
		
	filter "configurations:Debug"
		defines "DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "RELEASE"
		runtime "Release"
		optimize "on"
