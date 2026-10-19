#pragma once

class ColorSphereIndicator
{
public:

	ColorSphereIndicator(){}
	~ColorSphereIndicator() { Hide(); }

	// 解放を二重に行わないよう、コピーは禁止する
	ColorSphereIndicator(const ColorSphereIndicator&) = delete;
	ColorSphereIndicator& operator=(const ColorSphereIndicator&) = delete;

	// カラースフィアを表示する
	void Show()
	{
		// すでに表示中なら、何もしない
		if (m_handle != -1) { return; }

		m_handle = KdShaderManager::Instance().CreateColorSphere();
	}

	// カラースフィアを消す
	void Hide()
	{
		// 解放済み(または未表示)なら、何もしない
		// (解放済みの番号をもう一度解放すると、別の物が使っている番号を消してしまう)
		if (m_handle == -1) { return; }

		KdShaderManager::Instance().ReleaseColorSphere(m_handle);
		m_handle = -1;
	}

	// 四角形のカラースフィア表示
	void DrawRectangle(const Math::Vector3& pos,const Math::Vector3& dir,const Math::Vector2& size, const Math::Vector3& color=Math::Vector3(2.0f,0.0f,0.0f))
	{
		if (m_handle == -1) { return; }

		KdShaderManager::Instance().WriteCBColorSphereRectangle(m_handle, pos, dir, size,color);
	}


	// 円形のカラースフィア表示
	void DrawCircle(const Math::Vector3& pos,const float radius, const Math::Vector3& color = Math::Vector3(2.0f, 0.0f, 0.0f))
	{
		if (m_handle == -1) { return; }

		KdShaderManager::Instance().WriteCBColorSphereCircle(m_handle, pos, radius, color);
	}

	// 扇形のカラースフィア表示
	void DrawSector(const Math::Vector3& pos, const Math::Vector3& dir, const float radius,const float angleDeg, const Math::Vector3& color = Math::Vector3(2.0f, 0.0f, 0.0f))
	{
		if (m_handle == -1) { return; }

		KdShaderManager::Instance().WriteCBColorSphereSector(m_handle, pos, dir, radius, angleDeg, color);
	}


private:

	int m_handle = -1;

};