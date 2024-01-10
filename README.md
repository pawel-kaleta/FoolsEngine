# FoolsEngine

'FoolsEngine' is meant to be a lightwaight, but fairly powerful C++ game engine beeing written mostly as part of a game engine development learning process. It can be used as a technology demonstrator, an educational tool or a R&D framework.

![FoolsTools - FoolsEngine's editor](./documentation/FoolsTools.jpg)

## Disclamer
'FoolsEngine' is a prototype engine that is being developed in my spare time and as such there will likely be bugs. It is not intended as a stable production tech, but as a relativelly simple and clear framework for learning about how to build certain pieces of tech as well as for experimentation.
If your goal is to make a game, you are better of using off-the-shelf, fully featured, production ready game engine like Unreal/Unity/Godot.

* [Features](#Features)
* [Getting Started](#Getting-Started)
* [Language and project structure](#Language-and-project-structure)
* [Documentation](#Documentation)
* [Profiling data](#Profiling)
* [Technologies](#Technologies)
* [Sources](#Sources)

## Features
- Window creation and handling
- Buffered events system
- Input system
- Dear ImGui integration
- Debugging functionalities
	- logging
	- profiling
	- assertions
- Hybrid Actor/GameObject/ECS scene representation model
- Placeholder 2D renderer
- Editor features
	- Basic UI setup
	- Viewport and camera
	- Performance stats
	- Scene hierarchy vidget
	- Actor inspector
	- Entity inspector 

## Getting Started
`FoolsEngine` uses PreMake and Visual Studio 2022 for its build system.
Other development environments are untested and would require a custom setup.

1. Download the repository

Start by cloning the repository with `git clone --recursive https://github.com/pawel-kaleta/FoolsEngine`.

If the repository was cloned previously without recursion, use `git submodule update --init` to clone the necessary submodules.

2. Regenerate VS project files (optional)

If you want to regenerate project files (e.g. after making changes to build configuration files), run `GenerateProject.bat` script file in the root folder of the repository.

3. Compile and run.

Open `FoolsEngine.sln` VisualStudio solution file and press F5 to compile and run the editor.

Use (Ctrl+) Q,W,E,A,S,D) for controlling the camera.
Use arrow keys and numpad 1,3,4,6 keys to controll player's character representing test actor.

## Project structure
'FoolsEngine' is beeing developed in C++ with a cross-platfrom architecture design abstracting platform specific implementations and using only cross-platform external librarys. Currently only Windows and OpenGL implementations are being worked on.
Program structure isolates engine and application into separate exe/dll files.
- **Engine DLL** - `FoolsEngine`
	- Implements IoC (inversion of control) using layers stack
	- Includes entry point
	- Provides core layers: main application layer, ImGui layer
- **Application EXE** - `Sandbox` (outdated, most likely does not compile)
	- Extends main application layer
	- Provides additionall layers
	- May separate it's parts into hot-reloadable DLLs
- **Editor EXE** - `FoolsTools`
	- Provides editor-specific layers
- **Test EXE** - `TestBed` (planned)
	- Intended for testeing/debugging of the engine without running the game or editor

## Documentation

`FoolsEngine` architectual design is documented in `./documentation` folder.
- Event system
- Scene representation model
- Rendering (mostly planned)

## Profiling data

Build-in solution is an intrusive logging of profiling data into `.json` files. Profiling data can be visually inspected using Chorme build-in profiling tool - `chrome://tracing`.

There are automatic dedicated profiling sessions for startup, shutdown and first 30 frames of runtime. Additionall 30 frames long runtime session can be launched with `P` keybord shortcut.

![FoolsTools - FoolsEngine's editor](./documentation/Profiler.jpg)

## Technologies
- C++17
- VisualStudio 2022
- PreMake (config in LUA) 
- ImGui
- GLFW
- glad
- glm
- spdlog
- stb
- Chromium Trace Event Profiling Tool
- EnTT (ECS)
- OpenGL

## Sources
`FoolsEngine` development is inspired by and draws from various projects and tutorials of other people:
[![Youtube](https://img.shields.io/badge/The_Cherno_—_Game_Engine_Series--red.svg?style=social&logo=youtube)](https://www.youtube.com/playlist?list=PLlrATfBNZ98dC-V-N3m0Go4deliWHPFwT)
[![Youtube](https://img.shields.io/badge/Travis_Vroman_—_Vulkan_Game_Engine_Series_Written_in_C_(Kohi_Game_Engine)--red.svg?style=social&logo=youtube)](https://www.youtube.com/playlist?list=PLv8Ddw9K0JPg1BEO-RS-0MYs423cvLVtj)
[![Youtube](https://img.shields.io/badge/Bobby_Anguelov_—_Esoterica_(previously_Kruger)--red.svg?style=social&logo=youtube](https://www.youtube.com/@BobbyAnguelov/streams)
[![Youtube](https://img.shields.io/badge/Cem_Yuksel_—_Interactive_Computer_Graphics--red.svg?style=social&logo=youtube](https://www.youtube.com/playlist?list=PLplnkTzzqsZS3R5DjmCQsqupu43oS9CFN)
[![Youtube](https://img.shields.io/badge/Game_Engine_Series_—_Primal_Engine--red.svg?style=social&logo=youtube](www.youtube.com/@GameEngineSeries)
[![Youtube](https://img.shields.io/badge/Molly_Rocket_—_Handmade_Hero--red.svg?style=social&logo=youtube](https://www.youtube.com/@MollyRocket/featured)
[Learn OpenGL](https://learnopengl.com/)
[BitSquid blog] (https://bitsquid.blogspot.com/)
[VulkanGuide](https://vkguide.dev/)