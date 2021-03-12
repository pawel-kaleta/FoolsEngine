workspace "FoolsEngine"
	architecture "x64"

	configurations
	{
		"Debug",
		"Release",
		"Publish"
	}

outputdir = "%{cfg.buildcfg}_%{cfg.system}_%{cfg.architecture}"

project "FoolsEngine"
	location "FoolsEngine"
	kind "SharedLib"
	language "C++"

	targetdir ("bin/%{prj.name}_" .. outputdir)
	objdir ("bin-int/%{prj.name}_" .. outputdir)

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs
	{
		"%{prj.name}/externals/../include"
	}

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "latest"

		defines
		{
			"FE_PLATFORM_WINDOWS",
			"FE_BUILD_DLL"
		}

		--[[
		FoolsEngine is going to be a dll file required by the Sandbox app.

		postbuildcommands
		{
			("{COPY} %{cfg.buildtarget.relpath}../bin/" .. outputdir .. "/Sandbox")
		}
		]]

	filter "configurations:Debug"
		defines "FE_DEBUG"
		symbols "On"

	filter "configurations:Release"
		defines "FE_RELEASE"
		optimize "On"

	filter "configurations:Publish"
		defines "FE_PUBLISH"
		optimize "On"

project "Sandbox"
	location "Sandbox"
	kind "ConsoleApp"
	language "C++"

	targetdir ("bin/%{prj.name}_" .. outputdir)
	objdir ("bin-int/%{prj.name}_" .. outputdir)

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs
	{
		"FoolsEngine/externals/../include",
		"FoolsEngine/src"
	}

	links
	{
		"FoolsEngine"
	}

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "latest"

		defines
		{
			"FE_PLATFORM_WINDOWS",
			"FE_BUILD_DLL"
		}

	filter "configurations:Debug"
		defines "FE_DEBUG"
		symbols "On"

	filter "configurations:Release"
		defines "FE_RELEASE"
		optimize "On"

	filter "configurations:Publish"
		defines "FE_PUBLISH"
		optimize "On"
