#pragma once

#include "FoolsEngine/Platform/Win32/Win32Window.h"

#include "FoolsEngine/Renderer/1 - GAPI/Context.h"


struct InitContext
{
	void Init();
	void InitGlobal();

	void DeinitGlobal();
};