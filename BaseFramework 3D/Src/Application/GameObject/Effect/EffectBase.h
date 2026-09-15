#pragma once

class CameraBase;

class Player;

class EffectBase :public KdGameObject
{
public:

	EffectBase(){}
	~EffectBase()          override{}

	void Init()            override;
	void Update()          override;
	void DrawEffect()      override;

	void SetUpReference()	override;

	void SetTarget(std::shared_ptr<KdGameObject>character) { m_wpTarget = character; }

	void SetOffsetPos(const Math::Vector3& offset) { m_offsetPos = offset; }

	void UpdateBillboard();


protected:

	std::weak_ptr<CameraBase>   m_wpCamera;

	std::weak_ptr<KdGameObject>m_wpTarget;
	std::weak_ptr<KdGameObject>m_wpPlayer;



	std::shared_ptr<KdSquarePolygon>m_spPolygon=nullptr;

	Math::Vector3 m_offsetPos = Math::Vector3::Zero;

	float m_animCount = 0.0f;

};