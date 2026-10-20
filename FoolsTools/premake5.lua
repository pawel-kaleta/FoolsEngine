project "FoolsTools"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++20"
	staticruntime "on"
	externalwarnings "off"
	externalanglebrackets "on"
	exceptionhandling ("Off")

	buildoptions {
		"/utf-8"
	}

	targetdir ("../bin/"     .. outputdir .. "/%{prj.name}")
	objdir    ("../bin-int/" .. outputdir .. "/%{prj.name}")

	files {
		"src/**.h",
		"src/**.cpp"
	}

	includedirs {
		common_includedirs,
		"src"
	}

	links {
		"FoolsEngine"
	}

	defines {
		common_defines
	}

	filter "system:windows"
		systemversion "latest"
		defines	{
			"FE_PLATFORM_WINDOWS",
			"FE_PARAMS_PILE_SIZE=20*1024",
			"FE_PARAMS_HEADLESS=false",
			"FE_PARAMS_asd",
			"FE_PARAMS_sss",
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
			"FE_EDITOR"
		}
		runtime "Release"
		optimize "on"
		symbols "on"