workspace "GameEngineProject"
	architecture "x86_64"
	startproject "Sandbox"

	configurations
	{
		"Debug",
		"Dev",
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
IncludeDir["irrKlang"] = "Engine/dependencies/irrKlang/include"
IncludeDir["freetype2"] = "Engine/dependencies/freetype2/include"
IncludeDir["recast"] = "Engine/dependencies/recastnavigation/Recast/Include"
IncludeDir["detour"] = "Engine/dependencies/recastnavigation/Detour/Include"
IncludeDir["detour_tile_cache"] = "Engine/dependencies/recastnavigation/DetourTileCache/Include"
IncludeDir["recast_detour_debug"] = "Engine/dependencies/recastnavigation/DebugUtils/Include"

group "Dependencies"
	include "Engine/dependencies/Glad"
	include "Engine/dependencies/GLFW"
	include "Engine/dependencies/imgui"
group ""

group "recast_navigation"
	include "Engine/dependencies/recastnavigation"
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
		"%{IncludeDir.irrKlang}",
		"%{IncludeDir.freetype2}",
		"%{IncludeDir.recast}",
		"%{IncludeDir.detour}",
		"%{IncludeDir.detour_tile_cache}",
		"%{IncludeDir.recast_detour_debug}",
	}

	links 
	{
		"Glad",
		"GLFW",
		"ImGui",
		"Recast",
		"Detour",
		"DetourTileCache",
		"DebugUtils",
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

	filter "configurations:Dev"
		defines "DEBUG"
		runtime "Release"
		optimize "on"

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
		"%{IncludeDir.irrKlang}",
		"%{IncludeDir.freetype2}",
		"%{IncludeDir.recast}",
		"%{IncludeDir.detour}",
		"%{IncludeDir.detour_tile_cache}",
		"%{IncludeDir.recast_detour_debug}",
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
		"{COPY} ../%{prj.name}/res %{cfg.targetdir}/res",
		"{COPY} ../Engine/dependencies/assimp/lib/assimp-vc142-mt.dll %{cfg.targetdir}",
		"{COPY} ../Engine/dependencies/irrKlang/lib/*.dll %{cfg.targetdir}",
	}

	filter "system:windows"
		systemversion "latest"
		
	filter "configurations:Debug"
		defines "DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Dev"
		defines "DEBUG"
		runtime "Release"
		optimize "on"

	filter "configurations:Release"
		defines "RELEASE"
		runtime "Release"
		optimize "on"
