#pragma once

#include"../EditorPanel/EditorPanel.h"

class SceneView:public EditorPanel
{
public:

	SceneView():EditorPanel("Scene View"){}
	~SceneView()override {}


	void Init();

	void BeginRender();
	void EndRender();

	ImVec2 GetLocalMousePos()const;

	int GetWidth()const { return m_width; }
	int GetHeight()const { return m_height; }

	float GetAspectRatio()const;

	// ウィンドウにマウスが乗っているか
	bool IsHovered()const
	{
		// IsHoveredがtrueかつ
		// 現在SceneViewが描画されているか
		return m_isHovered &&
			m_lastDrawFrame == ImGui::GetFrameCount();
	}

	bool SizeChanged()const
	{
		return m_sizeChanged&&
			m_lastDrawFrame == ImGui::GetFrameCount();
	}

protected:

	void OnDraw()override;

	ImGuiWindowFlags GetImGuiWindowFlags()const override;

private:

	bool UpdateRenderTargetSize(const ImVec2& viewSize);

	KdRenderTargetPack    m_renderTarget;
	KdRenderTargetChanger m_renderTargetChanger;

	// SceneViewのサイズ
	int m_width = 0;
	int m_height = 0;

	ImVec2 m_viewPos = { 0.0f,0.0f };
	bool m_isHovered = false;
	
	// サイズ変更をしたか
	bool m_sizeChanged = false;


	// 現在のフレーム番号を入れる
	int m_lastDrawFrame = 0;


};