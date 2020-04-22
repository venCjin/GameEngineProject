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
IncludeDir["spdlog"] = "Engine/dependencies/spdlog/include"
IncludeDir["stb_image"] = "Engine/dependencies/stb_image"
IncludeDir["assimp"] = "Engine/dependencies/assimp/include"
-- IncludeDir["entityX"] = "Engine/dependencies/entityX"
IncludeDir["irrKlang"] = "Engine/dependencies/irrKlang/include"
IncludeDir["freetype2"] = "Engine/dependencies/freetype2/include"

group "Dependencies"
	-- include "Engine/dependencies/entityX"
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
		"%{prj.name}/src/**.c", 
		"%{prj.name}/src/**.hpp", 
		"%{prj.name}/src/**.cpp",
	}

	defines
	{
		"_CRT_SECURE_NO_WARNINGS",
	}

	includedirs
	{
		"%{prj.name}/src",
		-- "%{prj.name}/dependencies",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.spdlog}",
		"%{IncludeDir.stb_image}",
		"%{IncludeDir.assimp}",
		-- "%{IncludeDir.entityX}",
		"%{IncludeDir.irrKlang}",
		"%{IncludeDir.freetype2}",
	}

	links 
	{
		"Glad",
		"GLFW",
		"ImGui",
		-- "entityX",
		"opengl32.lib",
		"Engine/dependencies/assimp/lib/assimp-vc142-mt.lib",
		"Engine/dependencies/irrKlang/lib/irrKlang.lib",
		"Engine/dependencies/freetype2/lib/freetype.lib",
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
		"%{prj.name}/src/**.c", 
		"%{prj.name}/src/**.hpp", 
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/res/**",
	}

	includedirs
	{
		"Engine/src",
		-- "Engine/dependencies",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.spdlog}",
		"%{IncludeDir.stb_image}",
		"%{IncludeDir.assimp}",
		-- "%{IncludeDir.entityX}",
		"%{IncludeDir.irrKlang}",
		"%{IncludeDir.freetype2}",
	}

	links
	{
		"Engine",
		"Engine/dependencies/assimp/lib/assimp-vc142-mt.lib",
		"Engine/dependencies/irrKlang/lib/irrKlang.lib",
		"Engine/dependencies/freetype2/lib/freetype.lib",
	}

	postbuildcommands 
	{
		"{COPY} ../Game/res %{cfg.targetdir}/res",
		"{COPY} ../Engine/dependencies/assimp/lib/assimp-vc142-mt.dll %{cfg.targetdir}",
		"{COPY} ../Engine/dependencies/irrKlang/lib/*.dll %{cfg.targetdir}",
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
