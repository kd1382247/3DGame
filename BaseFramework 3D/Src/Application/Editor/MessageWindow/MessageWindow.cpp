#include "MessageWindow.h"

void MessageWindow::Open(const std::string& title, const std::string& message)
{
	m_title = title;
	m_message = message;

	m_requestOpen = true;

}

void MessageWindow::Draw()
{

	if (m_requestOpen)
	{
		ImGui::OpenPopup(m_title.c_str());

		m_requestOpen = false;
	}

	if (!ImGui::BeginPopupModal(m_title.c_str(), nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		return;
	}

	// メッセージを表示
	
	ImGui::Text("%s",m_message.c_str());

	// 閉じる
	if (ImGui::Button("OK"))
	{
		ImGui::CloseCurrentPopup();
	}

	ImGui::EndPopup();

}
