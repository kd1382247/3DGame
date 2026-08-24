#pragma once

class CameraBase;

class FlyText:public KdGameObject
{
public:

	FlyText(){}
	~FlyText(){}
	 
	void Init(int damage, const Math::Vector3& spawnPos);
	void Update();
	void DrawEffect()override;

	void SetUpReference()override;

private:

	void DrawDigit(int digit, float xOffset);

	std::weak_ptr<CameraBase>m_wpCamera;

	std::shared_ptr<KdSquarePolygon>m_spPolygon = nullptr;

	std::string m_text = {};

	float m_lifeTime = 30.0f;        // 残り時間(秒でもフレームでもOK)

	float m_moveSpeed = 0.03f;      // 上昇速度

	float m_alpha = 1.0f;

};