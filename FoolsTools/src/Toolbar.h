#pragma once

#include <FoolsEngine.h>
#include "EditorState.h"

#include "EditorAssetHandle.h"

namespace fe
{
	enum class ToolbarButton
	{
		None = 0,
		Play,
		Pause,
		Stop
	};

	class Toolbar
	{
	public:
		Toolbar();

		void OnImGuiRender();
		ToolbarButton GetClickedButton() { return m_ClickedButton; }
		void SetEditorState(EditorState state) { m_EditorState = state; }
	private:
		ToolbarButton m_ClickedButton = ToolbarButton::None;
		EditorState m_EditorState;

		EditorAssetHandle m_IconPlay;
		UInt mIconPlayID;
		EditorAssetHandle m_IconPause;
		UInt mIconPauseID;
		EditorAssetHandle m_IconStop;
		UInt mIconStopID;
	};
}