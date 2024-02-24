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

group "Dependencies"
	include "FoolsEngine/externals"
group ""

include "FoolsEngine"
include "FoolsTools"
include "Sandbox"

