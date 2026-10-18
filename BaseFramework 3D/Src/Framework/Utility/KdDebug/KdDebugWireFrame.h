#pragma once

class KdDebugWireFrame
{
public:
	KdDebugWireFrame() {}
	~KdDebugWireFrame() { Release(); }

	void AddDebugLine(const Math::Vector3& start, const Math::Vector3& end, const Math::Color& col = kWhiteColor);
	void AddDebugLine(const Math::Vector3& start, const Math::Vector3& dir = Math::Vector3(0,0,1), float length = 1.0f, const Math::Color& col = kWhiteColor);
	void AddDebugLineFromMatrix(const Math::Matrix& mat, float scale);
	void AddDebugSphere(const Math::Vector3& pos, float radius, const Math::Color& col = kWhiteColor);
	void AddDebugBox(const Math::Matrix& matrix, const Math::Vector3& size, const Math::Vector3& offset = { 0, 0, 0 }, const bool isOriented = false, const Math::Color& col = kWhiteColor);

	void Draw();

	// 全てのデバッグ線の追加を、まとめてON/OFFする
	// OFFの間は線を追加しても何もしない (描画しないまま頂点が溜まり続けるのを防ぐため)
	static void SetEnable(const bool isEnable) { s_isEnable = isEnable; }
	static bool IsEnable() { return s_isEnable; }

private:
	void Release();

	// デバッグ線を追加するか (全てのKdDebugWireFrameで共通)
	static inline bool s_isEnable = true;

	std::vector<KdPolygon::Vertex>	m_debugVertices;
};
