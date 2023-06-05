project "FoolsTools"
	location "FoolsTools"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("../bin/"     .. outputdir .. "/%{prj.name}")
	objdir    ("../bin-int/" .. outputdir .. "/%{prj.name}")

	files {
		"src/**.h",
		"src/**.cpp"
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
			"FE_INTERNAL_BUILD",
			"FE_EDITOR"
		}
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines {
			"FE_RELEASE",
			"FE_INTERNAL_BUILD",
			"FE_EDITOR"
		}
		runtime "Release"
		optimize "on"
		symbols "on"

	filter "configurations:Publish"
		defines {
			"FE_PUBLISH",
			"FE_INTERNAL_BUILD",
			"FE_EDITOR"
		}
		runtime "Release"
		optimize "on"
		symbols "on"