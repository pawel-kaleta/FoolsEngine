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
	cppdialect "C++20"

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

project "ImGuizmo"
	location "_projects_of_externals/%{prj.name}"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"

	targetdir ("../../bin/"     .. outputdir .. "/externals/%{prj.name}")
	objdir    ("../../bin-int/" .. outputdir .. "/externals/%{prj.name}")

	files {
		"%{prj.name}/*.h",
		"%{prj.name}/*.cpp"
	}

	includedirs	{
		"imgui"
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

project "assimp"
	-- when updating the submodule reconfigure the following files (can use CMake to do it automatically)
	-- _projects_of_externals/assimp/revision.h
	-- _projects_of_externals/assimp/include/assimp/config.h
	-- _projects_of_externals/assimp/contrib/zlib/zconf.h
	location "_projects_of_externals/%{prj.name}"
	kind "StaticLib"
	warnings "Off"
	optimize "Speed"
	cppdialect "C++17"

	targetdir ("../../bin/"     .. outputdir .. "/externals/%{prj.name}")
	objdir    ("../../bin-int/" .. outputdir .. "/externals/%{prj.name}")

	includedirs {
		'_projects_of_externals/%{prj.name}',
		'_projects_of_externals/%{prj.name}/include',
		'_projects_of_externals/%{prj.name}/contrib/zlib',
		'assimp',
		'assimp/contrib',
		'assimp/contrib/unzip',
		'assimp/contrib/zlib',
		'assimp/contrib/rapidjson/include',
		'assimp/contrib/utf8cpp/source',
		'assimp/include',
		'assimp/code'
	}

	files {
		-- Dependencies
		'assimp/contrib/unzip/**',
		'assimp/contrib/zlib/*',
		-- Common
		'assimp/code/CApi/**',
		'assimp/code/Common/**',
		'assimp/code/Geometry/**',
		'assimp/code/Material/**',
		'assimp/code/Pbrt/**',
		'assimp/code/PostProcessing/**',
		-- Importers and Exporters
		'assimp/code/AssetLib/Obj/**',
		'assimp/code/AssetLib/FBX/**',
		'assimp/code/AssetLib/glTF/**.cpp', 
		'assimp/code/AssetLib/glTF/**.h', -- glTF has some .inl files, we are skipping them to prevent compiling them like .cpp files
		'assimp/code/AssetLib/glTF2/**.cpp', 
		'assimp/code/AssetLib/glTF2/**.h', -- glTF has some .inl files, we are skipping them to prevent compiling them like .cpp files
		'assimp/code/AssetLib/Assbin/**' -- For caching
	}
	-- Importers
	defines {
		'RAPIDJSON_HAS_STDSTRING=1',

		'ASSIMP_BUILD_NO_3D_IMPORTER',
		'ASSIMP_BUILD_NO_3DS_IMPORTER',
		'ASSIMP_BUILD_NO_3MF_IMPORTER',
		'ASSIMP_BUILD_NO_AC_IMPORTER',
		'ASSIMP_BUILD_NO_AMF_IMPORTER',
		'ASSIMP_BUILD_NO_ASE_IMPORTER',
		-- 'ASSIMP_BUILD_NO_ASSBIN_IMPORTER',
		'ASSIMP_BUILD_NO_B3D_IMPORTER',
		'ASSIMP_BUILD_NO_BLEND_IMPORTER',
		'ASSIMP_BUILD_NO_BVH_IMPORTER',
		'ASSIMP_BUILD_NO_C4D_IMPORTER',
		'ASSIMP_BUILD_NO_COB_IMPORTER',
		'ASSIMP_BUILD_NO_COLLADA_IMPORTER',
		'ASSIMP_BUILD_NO_CSM_IMPORTER',
		'ASSIMP_BUILD_NO_DXF_IMPORTER',
		-- 'ASSIMP_BUILD_NO_FBX_IMPORTER',
		-- 'ASSIMP_BUILD_NO_GLTF_IMPORTER',
		'ASSIMP_BUILD_NO_HMP_IMPORTER',
		'ASSIMP_BUILD_NO_IFC_IMPORTER',
		'ASSIMP_BUILD_NO_IQM_IMPORTER',
		'ASSIMP_BUILD_NO_IRR_IMPORTER',
		'ASSIMP_BUILD_NO_IRRMESH_IMPORTER',
		'ASSIMP_BUILD_NO_LWO_IMPORTER',
		'ASSIMP_BUILD_NO_LWS_IMPORTER',
		'ASSIMP_BUILD_NO_M3D_IMPORTER',
		'ASSIMP_BUILD_NO_MD2_IMPORTER',
		'ASSIMP_BUILD_NO_MD3_IMPORTER',
		'ASSIMP_BUILD_NO_MD5_IMPORTER',
		'ASSIMP_BUILD_NO_MDC_IMPORTER',
		'ASSIMP_BUILD_NO_MDL_IMPORTER',
		'ASSIMP_BUILD_NO_MMD_IMPORTER',
		'ASSIMP_BUILD_NO_MS3D_IMPORTER',
		'ASSIMP_BUILD_NO_NDO_IMPORTER',
		'ASSIMP_BUILD_NO_NFF_IMPORTER',
		-- 'ASSIMP_BUILD_NO_OBJ_IMPORTER',
		'ASSIMP_BUILD_NO_OFF_IMPORTER',
		'ASSIMP_BUILD_NO_OGRE_IMPORTER',
		'ASSIMP_BUILD_NO_OPENGEX_IMPORTER',
		'ASSIMP_BUILD_NO_PLY_IMPORTER',
		'ASSIMP_BUILD_NO_Q3BSP_IMPORTER',
		'ASSIMP_BUILD_NO_Q3D_IMPORTER',
		'ASSIMP_BUILD_NO_RAW_IMPORTER',
		'ASSIMP_BUILD_NO_SIB_IMPORTER',
		'ASSIMP_BUILD_NO_SMD_IMPORTER',
		'ASSIMP_BUILD_NO_STEP_IMPORTER',
		'ASSIMP_BUILD_NO_STL_IMPORTER',
		'ASSIMP_BUILD_NO_TERRAGEN_IMPORTER',
		'ASSIMP_BUILD_NO_X_IMPORTER',
		'ASSIMP_BUILD_NO_X3D_IMPORTER',
		'ASSIMP_BUILD_NO_XGL_IMPORTER'
	}
	-- Exporters
	defines {
		'ASSIMP_BUILD_NO_COLLADA_EXPORTER',
		'ASSIMP_BUILD_NO_X_EXPORTER',
		'ASSIMP_BUILD_NO_STEP_EXPORTER',
		'ASSIMP_BUILD_NO_OBJ_EXPORTER',
		'ASSIMP_BUILD_NO_STL_EXPORTER',
		'ASSIMP_BUILD_NO_PLY_EXPORTER',
		'ASSIMP_BUILD_NO_3DS_EXPORTER',
		'ASSIMP_BUILD_NO_GLTF_EXPORTER',
		-- 'ASSIMP_BUILD_NO_ASSBIN_EXPORTER',
		'ASSIMP_BUILD_NO_ASSXML_EXPORTER',
		'ASSIMP_BUILD_NO_X3D_EXPORTER',
		'ASSIMP_BUILD_NO_FBX_EXPORTER',
		'ASSIMP_BUILD_NO_M3D_EXPORTER',
		'ASSIMP_BUILD_NO_3MF_EXPORTER',
		'ASSIMP_BUILD_NO_ASSJSON_EXPORTER'
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