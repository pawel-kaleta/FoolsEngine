workspace "FoolsEngine"
	architecture "x86_64"

	configurations {
		"Debug",
		"Release",
		"Publish"
	}

	startproject "FoolsTools"

	flags {
		"MultiProcessorCompile"
	}

outputdir = "%{cfg.buildcfg}_%{cfg.system}_%{cfg.architecture}"
common_includedirs = {
	"../FoolsEngine/src",
	"../FoolsEngine/externals/spdlog/include",
	"../FoolsEngine/externals/glfw/include",
	"../FoolsEngine/externals/glad/include",
	"../FoolsEngine/externals/imgui",
	"../FoolsEngine/externals/glm",
	"../FoolsEngine/externals/stb",
	"../FoolsEngine/externals/EnTT/src",
	"../FoolsEngine/externals/yaml-cpp/include",
	"../FoolsEngine/externals/ImGuizmo",
	"../FoolsEngine/externals/assimp/include",
	"../FoolsEngine/externals/_projects_of_externals/assimp/include"
}

common_defines = {
	"YAML_CPP_STATIC_DEFINE",
	"FE_EDITOR",
	"FE_PARAMS_PILE_SIZE=20*1024",
	"FE_PARAMS_HEADLESS=false",
	"FE_PARAMS_MAIN_ARENA_SIZE=32*1024",
	"FE_PARAMS_GAPI=OpenGL",
	"FE_PARAMS_GPU_SCRATCH_BUFFER_SIZE=32*1024",
	"FE_PARAMS_NAME=\"FoolsEngine\""
}

group "Dependencies"
	include "FoolsEngine/externals"
group ""

include "FoolsEngine"
include "FoolsTools"
include "Sandbox"

