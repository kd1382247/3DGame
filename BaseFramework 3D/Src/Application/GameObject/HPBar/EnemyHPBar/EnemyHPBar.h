#pragma once

class CameraBase;

class EnemyHPBar:public KdGameObject
{
public:
	
	EnemyHPBar(){}
	~EnemyHPBar()override{}

	void Init()override;
	void Update()override;
	void DrawEffect()override;

	void SetTarget(const std::shared_ptr<KdGameObject>enemy) { m_wpTarget = enemy; }

	void SetOffsetPos(const Math::Vector3& offsetPos) { m_offsetPos = offsetPos; }

	void SetUpReference()override;

private:

	void DrawBackground();
	void DrawDamageBar();
	void DrawHPBar();
	
	void DrawBar(const std::shared_ptr<KdSquarePolygon>& polygon, float rate, float depth);

	Math::Matrix CreateBaseMatrix()const;

	std::weak_ptr<CameraBase>m_wpCamera;
	std::weak_ptr<KdGameObject>m_wpTarget;
	std::weak_ptr<KdGameObject>m_wpPlayer;

	std::shared_ptr<KdSquarePolygon>m_spBackground = nullptr;
	std::shared_ptr<KdSquarePolygon>m_spDamageBar = nullptr;
	std::shared_ptr<KdSquarePolygon>m_spHPBar = nullptr;

	float m_hpRate = 1.0f;      // 緑
	float m_damageRate = 1.0f;  // 赤

	float m_barWidth = 1.5f;    // 横幅
	float m_barHeight = 0.2f;

	// Barの位置
	Math::Vector3 m_offsetPos = Math::Vector3::Zero;

	bool          m_isHPBarVisible=false;

	static constexpr float DamageBarDepth = -0.0001f;
	static constexpr float HPBarDepth = -0.0002f;
};