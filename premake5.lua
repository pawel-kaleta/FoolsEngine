workspace "FoolsEngine"
	architecture "x64"

	configurations {
		"Debug",
		"Release",
		"Publish"
	}

outputdir = "%{cfg.buildcfg}_%{cfg.system}_%{cfg.architecture}"

project "FoolsEngine"
	location "FoolsEngine"
	kind "SharedLib"
	language "C++"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files {
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs	{
		"FoolsEngine/src",
		"FoolsEngine/externals/spdlog/include"
	}

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "latest"

		defines	{
			"FE_PLATFORM_WINDOWS",
			"FE_BUILD_DLL"
		}

		postbuildcommands {
			"{mkdir} ../bin/" .. outputdir .. "/Sandbox",
			"{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/Sandbox"
		}

	filter "configurations:Debug"
		defines {
			"FE_DEBUG",
			"FE_INTERNAL_BUILD"
		}
		symbols "On"

	filter "configurations:Release"
		defines {
			"FE_RELEASE",
			"FE_INTERNAL_BUILD"
		}
		optimize "On"

	filter "configurations:Publish"
		defines "FE_PUBLISH"
		optimize "On"

project "Sandbox"
	location "Sandbox"
	kind "ConsoleApp"
	language "C++"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files {
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs {
		"FoolsEngine/src",
		"FoolsEngine/externals/spdlog/include"
	}

	links {
		"FoolsEngine"
	}

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "latest"

		defines	{
			"FE_PLATFORM_WINDOWS"
		}

	filter "configurations:Debug"
		defines {
			"FE_DEBUG",
			"FE_INTERNAL_BUILD"
		}
		symbols "On"

	filter "configurations:Release"
		defines {
			"FE_RELEASE",
			"FE_INTERNAL_BUILD"
		}
		optimize "On"

	filter "configurations:Publish"
		defines "FE_PUBLISH"
		optimize "On"