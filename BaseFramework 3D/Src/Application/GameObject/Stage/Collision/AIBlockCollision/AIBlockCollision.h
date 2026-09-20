#pragma once

class AIBlockCollision :public KdGameObject
{

public:
	AIBlockCollision() {}
	~AIBlockCollision()override {}

	void Init()override;
	void DrawLit()override {}

	void SetMatrix(const Math::Matrix& mat) { m_mWorld = mat; }

	// 読み込むモデルのパスを指定する(Init()より前に呼ぶこと)
	void SetModelPath(const std::filesystem::path& path) { m_modelPath = path; }

private:


	std::shared_ptr<KdModelWork>m_spModel = nullptr;

	std::filesystem::path m_modelPath = "Asset/Models/Stage/Stage01/AIBlock.gltf";

};
