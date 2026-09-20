#pragma once

class AIBlockCollision :public KdGameObject
{

public:
	AIBlockCollision() {}
	~AIBlockCollision()override {}

	void Init()override;
	void DrawLit()override {}

	void SetMatrix(const Math::Matrix& mat) { m_mWorld = mat; }

private:


	std::shared_ptr<KdModelWork>m_spModel = nullptr;

};
