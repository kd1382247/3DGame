#pragma once

class GroundCollision:public KdGameObject
{

public:
	GroundCollision() {}
	~GroundCollision()override{}

	void Init()override;

	void SetMatrix(Math::Matrix mat) { m_mWorld = mat; }

	// 読み込むモデルのパスを指定する(Init()より前に呼ぶこと)
	void SetModelPath(const std::filesystem::path& path) { m_modelPath = path; }

private:

	std::shared_ptr<KdModelWork>m_spModel = nullptr;

	std::filesystem::path m_modelPath = "Asset/Models/Stage/Stage01/GroundCollision.gltf";
};
