#pragma once

class Player;

// Mageの魔法円攻撃の当たり判定専用オブジェクト
// (発生してから一定時間(予備動作)経過後に一度だけ判定し、消える)
class MageMagicCircle :public KdGameObject
{
public:

	MageMagicCircle() {}
	~MageMagicCircle()override {}

	void Init()override;
	void Update()override;

	void SetUpReference()override;

	// 魔法円の設定(座標・半径・発動までの予備動作時間(秒)・ダメージ・ノックバックの強さ)
	void Setup(const Math::Vector3& pos, float radius, float telegraphTime, float damage, float knockBackPower);

private:

	std::weak_ptr<Player> m_wpPlayer;

	float m_radius = 1.0f;

	// 発動までの残り予備動作時間(秒)
	float m_telegraphTime = 0.0f;

	float m_damage = 0.0f;
	float m_knockBackPower = 0.0f;

	bool m_hitTarget = false;

	// 当たり判定を持続させるフレーム数(判定漏れ防止の余裕)
	float m_lifeTime = 4.0f;
};
