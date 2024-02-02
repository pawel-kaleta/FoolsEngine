project "FoolsEngine"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"
	externalwarnings "off"
	externalanglebrackets "on"

	targetdir ("../bin/"     .. outputdir .. "/%{prj.name}")
	objdir    ("../bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "FE_pch.h"
	pchsource "src/FE_pch.cpp"

	files {
		"src/**.h",
		"src/**.cpp"
	}

	includedirs	{
		common_includedirs
	}

	links {
		"glfw",
		"glad",
		"imgui",
		"stb",
		"yaml-cpp",
		"ImGuizmo"
	}

	buildoptions {
		"/analyze:external-",
	}

	defines "YAML_CPP_STATIC_DEFINE"

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