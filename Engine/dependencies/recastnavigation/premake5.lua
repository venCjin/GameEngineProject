project "DebugUtils"
	language "C++"
	kind "StaticLib"
	staticruntime "on"
    
    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	includedirs { 
		"DebugUtils/Include",
		"Detour/Include",
		"DetourTileCache/Include",
		"Recast/Include"
	}

	files { 
		"DebugUtils/Include/*.h",
		"DebugUtils/Source/*.cpp"
	}

	filter "system:windows"
        systemversion "latest"

    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        runtime "Release"
		optimize "on"
		
project "Detour"
	language "C++"
	kind "StaticLib"
    staticruntime "on"
    
    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	includedirs { 
		"Detour/Include" 
	}

	files { 
		"Detour/Include/*.h", 
		"Detour/Source/*.cpp" 
	}

	filter "system:windows"
		systemversion "latest"

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"

project "DetourTileCache"
	language "C++"
	kind "StaticLib"
	staticruntime "on"
    
    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	includedirs {
		"DetourTileCache/Include",
		"Detour/Include",
		"Recast/Include"
	}
	
	files {
		"DetourTileCache/Include/*.h",
		"DetourTileCache/Source/*.cpp"
	}
	
	filter "system:windows"
		systemversion "latest"

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"

project "Recast"
	language "C++"
	kind "StaticLib"
    staticruntime "on"
    
    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	includedirs { 
		"Recast/Include" 
	}

	files { 
		"Recast/Include/*.h",
		"Recast/Source/*.cpp" 
	}

	filter "system:windows"
        systemversion "latest"

    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        runtime "Release"
        optimize "on"