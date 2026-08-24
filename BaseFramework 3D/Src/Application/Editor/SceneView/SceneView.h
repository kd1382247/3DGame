#pragma once

class SceneView
{
public:

	void Init();
	void Draw();

	void BeginRender();
	void EndRender();

	ImVec2 GetLocalMousePos()const;

	int GetWidth()const { return m_width; }
	int GetHeight()const { return m_height; }

	float GetAspectRatio()const;

	bool IsHovered()const
	{
		return m_isHovered;
	}

	bool SizeChanged()const { return m_sizeChanged; }

private:

	bool UpdateRenderTargetSize(const ImVec2& viewSize);

	KdRenderTargetPack    m_renderTarget;
	KdRenderTargetChanger m_renderTargetChanger;

	// SceneViewのサイズ
	int m_width = 0;
	int m_height = 0;

	ImVec2 m_viewPos = { 0.0f,0.0f };
	ImVec2 m_viewSize = { 0.0f,0.0f };
	bool m_isHovered = false;
	
	// サイズ変更をしたか
	bool m_sizeChanged = false;


};