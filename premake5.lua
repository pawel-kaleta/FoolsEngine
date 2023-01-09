workspace "FoolsEngine"
	architecture "x64"

	configurations {
		"Debug",
		"Release",
		"Publish"
	}

	startproject "Sandbox"

outputdir = "%{cfg.buildcfg}_%{cfg.system}_%{cfg.architecture}"
common_includedirs = {
	"FoolsEngine/src",
	"FoolsEngine/externals/spdlog/include",
	"FoolsEngine/externals/glfw/include",
	"FoolsEngine/externals/glad/include",
	"FoolsEngine/externals/imgui",
	"FoolsEngine/externals/glm"
}

group "Dependencies"
	include "FoolsEngine/externals"
group ""

project "FoolsEngine"
	location "FoolsEngine"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"
	externalwarnings "off"
	externalanglebrackets "on"

	targetdir ("bin/"     .. outputdir .. "/%{prj.name}")
	objdir    ("bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "FE_pch.h"
	pchsource "FoolsEngine/src/FE_pch.cpp"

	files {
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs	{
		common_includedirs
	}

	links {
		"glfw",
		"glad",
		"imgui"
	}

	buildoptions {
		"/analyze:external-",
	}

	filter "system:windows"
		systemversion "latest"

		defines	{
			"FE_PLATFORM_WINDOWS",
			"GLFW_INCLUDE_NONE"
		}

	filter "configurations:Debug"
		defines {
			"FE_DEBUG",
			"FE_INTERNAL_BUILD"
		}
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines {
			"FE_RELEASE",
			"FE_INTERNAL_BUILD"
		}
		runtime "Release"
		optimize "on"

	filter "configurations:Publish"
		defines "FE_PUBLISH"
		runtime "Release"
		optimize "on"

project "Sandbox"
	location "Sandbox"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("bin/"     .. outputdir .. "/%{prj.name}")
	objdir    ("bin-int/" .. outputdir .. "/%{prj.name}")

	files {
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs {
		common_includedirs
	}

	links {
		"FoolsEngine"
	}

	filter "system:windows"
		systemversion "latest"
		defines	{
			"FE_PLATFORM_WINDOWS",
			"GLFW_INCLUDE_NONE"
		}

	filter "configurations:Debug"
		defines {
			"FE_DEBUG",
			"FE_INTERNAL_BUILD"
		}
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines {
			"FE_RELEASE",
			"FE_INTERNAL_BUILD"
		}
		runtime "Release"
		optimize "on"
		symbols "on"

	filter "configurations:Publish"
		defines "FE_PUBLISH"
		runtime "Release"
		optimize "on"
		symbols "on"