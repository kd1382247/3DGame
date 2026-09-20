#pragma once

class GroundCollision:public KdGameObject
{

public:
	GroundCollision() {}
	~GroundCollision()override{}

	void Init()override;

	void SetMatrix(Math::Matrix mat) { m_mWorld = mat; }

private:

	std::shared_ptr<KdModelWork>m_spModel = nullptr;
};
