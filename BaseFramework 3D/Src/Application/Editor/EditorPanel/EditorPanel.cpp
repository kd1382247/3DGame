#include "EditorPanel.h"

void EditorPanel::Draw()
{
	if (!m_isVisible)
	{
		return;
	}

	if(ImGui::Begin(m_windowName.c_str(),&m_isVisible,GetImGuiWindowFlags()))
	{
		OnDraw();
	}

	ImGui::End();

}
