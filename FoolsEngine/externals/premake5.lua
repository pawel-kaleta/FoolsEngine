project "glfw"
	location "_projects_of_externals/glfw"
	kind "StaticLib"
	language "C"

	targetdir ("../../bin/"     .. outputdir .. "/externals/%{prj.name}")
	objdir    ("../../bin-int/" .. outputdir .. "/externals/%{prj.name}")

	files {
		"%{prj.name}/include/GLFW/glfw3.h",
		"%{prj.name}/include/GLFW/glfw3native.h",
		"%{prj.name}/src/internal.h",
		"%{prj.name}/src/mappings.h",
		"%{prj.name}/src/platform.h",
		"%{prj.name}/src/platform.c",
		"%{prj.name}/src/context.c",
		"%{prj.name}/src/glfw_config.h",
		"%{prj.name}/src/egl_context.c",
		"%{prj.name}/src/context.c",
		"%{prj.name}/src/init.c",
		"%{prj.name}/src/input.c",
		"%{prj.name}/src/monitor.c",
		"%{prj.name}/src/vulkan.c",
		"%{prj.name}/src/window.c",
		"%{prj.name}/src/osmesa_context.c",
		"%{prj.name}/src/null_platform.h",
		"%{prj.name}/src/null_joystick.h",
		"%{prj.name}/src/null_init.c",
		"%{prj.name}/src/null_monitor.c",
		"%{prj.name}/src/null_window.c",
		"%{prj.name}/src/null_joystick.c"
	}

	filter "system:windows"
		systemversion "latest"
		staticruntime "on"

		defines	{
			"_GLFW_WIN32",
			"_CRT_SECURE_NO_WARNINGS"
		}

		files {
			"%{prj.name}/src/win32_init.c",
			"%{prj.name}/src/win32_module.c",
			"%{prj.name}/src/win32_joystick.h",
			"%{prj.name}/src/win32_joystick.c",
			"%{prj.name}/src/win32_monitor.c",
			"%{prj.name}/src/win32_time.h",
			"%{prj.name}/src/win32_time.c",
			"%{prj.name}/src/win32_thread.h",
			"%{prj.name}/src/win32_thread.c",
			"%{prj.name}/src/win32_window.c",
			"%{prj.name}/src/wgl_context.c",
			"%{prj.name}/src/win32_platform.h"
		}

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"

project "glad"
	location "_projects_of_externals/glad"
	kind "StaticLib"
	language "C"

	targetdir ("../../bin/"     .. outputdir .. "/externals/%{prj.name}")
	objdir    ("../../bin-int/" .. outputdir .. "/externals/%{prj.name}")

	files {
		"%{prj.name}/include/glad/glad.h",
		"%{prj.name}/include/KHR/khrplatform.h",
		"%{prj.name}/src/glad.c"
	}

	includedirs	{
		"%{prj.name}/include"
	}


	filter "system:windows"
		systemversion "latest"
		staticruntime "on"

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"

project "imgui"
	location "_projects_of_externals/imgui"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"

	targetdir ("../../bin/"     .. outputdir .. "/externals/%{prj.name}")
	objdir    ("../../bin-int/" .. outputdir .. "/externals/%{prj.name}")

	files {
		"%{prj.name}/imconfig.h",
		"%{prj.name}/imgui.h",
		"%{prj.name}/imgui.cpp",
		"%{prj.name}/imgui_draw.cpp",
		"%{prj.name}/imgui_internal.h",
		"%{prj.name}/imgui_tables.cpp",
		"%{prj.name}/imgui_widgets.cpp",
		"%{prj.name}/imstb_rectpack.h",
		"%{prj.name}/imstb_textedit.h",
		"%{prj.name}/imstb_truetype.h",
		"%{prj.name}/imgui_demo.cpp"
	}

	filter "system:windows"
		systemversion "latest"
		staticruntime "on"

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"

project "stb"
	location "_projects_of_externals/stb"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"

	targetdir ("../../bin/"     .. outputdir .. "/externals/%{prj.name}")
	objdir    ("../../bin-int/" .. outputdir .. "/externals/%{prj.name}")

	files {
		"%{prj.name}/**.h",
		"%{prj.name}/**.cpp"
	}

	filter "system:windows"
		systemversion "latest"
		staticruntime "on"

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"

project "yaml-cpp"
	location "_projects_of_externals/yaml-cpp"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"

	targetdir ("../../bin/"     .. outputdir .. "/externals/%{prj.name}")
	objdir    ("../../bin-int/" .. outputdir .. "/externals/%{prj.name}")

	files {
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",

		"%{prj.name}/include/yaml-cpp/**.h"
	}

	includedirs	{
		"%{prj.name}/include"
	}

	defines "YAML_CPP_STATIC_DEFINE"

	filter "system:windows"
		systemversion "latest"
		staticruntime "on"

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"