#include "SceneView.h"

void SceneView::Init()
{
	m_width = 800;
	m_height = 600;

	m_renderTarget.CreateRenderTarget(m_width, m_height, true);
}

void SceneView::Draw()
{

	static int count = 0;

	OutputDebugStringA("SceneView::Draw\n");


	ImGuiWindowFlags flags =
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoScrollbar |
		ImGuiWindowFlags_NoScrollWithMouse;

	ImGui::Begin("Scene View",nullptr,flags);

	// SceneView内で使えるサイズを取得
	ImVec2 newViewSize =ImGui::GetContentRegionAvail();


	m_sizeChanged=UpdateRenderTargetSize(newViewSize);
	
	// SceneView画像の左上座標
	m_viewPos = ImGui::GetCursorScreenPos();


	if(m_renderTarget.m_RTTexture)
	{
		ImGui::Image(
			(ImTextureID)m_renderTarget.m_RTTexture->WorkSRView(),
			newViewSize);

		// Imageにマウスが乗っているか
		m_isHovered = ImGui::IsItemHovered();
	}


	ImGui::End();
}

bool SceneView::UpdateRenderTargetSize(const ImVec2& viewSize)
{

	int width = static_cast<int>(viewSize.x);
	int height = static_cast<int>(viewSize.y);

	if (width <= 0 || height <= 0)
	{
		return false;
	}

	if (m_width == width && m_height == height)
	{
		return false;
	}

	m_width = width;
	m_height = height;

	m_renderTarget.CreateRenderTarget(m_width,m_height,true);

	return true;
}

void SceneView::BeginRender()
{
	// SceneView用RenderTargetへ変更
	if (!m_renderTargetChanger.ChangeRenderTarget(m_renderTarget))
	{
		
		return;
	}

	// 描画前にTextureとZBufferをクリア
	m_renderTarget.ClearTexture();
}

void SceneView::EndRender()
{
	// 元のRenderTargetへ戻す
	m_renderTargetChanger.UndoRenderTarget();

}

ImVec2 SceneView::GetLocalMousePos() const
{
	ImVec2 mousePos = ImGui::GetMousePos();

	return
	{
		mousePos.x - m_viewPos.x,
		mousePos.y - m_viewPos.y
	};

}

float SceneView::GetAspectRatio() const
{
	if (m_height <= 0)
	{
		return 1.0f;
	}

	return static_cast<float>(m_width) / static_cast<float>(m_height);

}

