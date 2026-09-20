#pragma once

class Player;

// 爆発の当たり判定専用オブジェクト
// (Bombなど、爆発した瞬間に消えるキャラクターに代わって、爆発の当たり判定を担当する)
class Explosion :public KdGameObject
{
public:

	Explosion() {}
	~Explosion()override {}

	void Init()override;
	void Update()override;

	void SetUpReference()override;

	// 爆発の設定(座標・半径・ダメージ・ノックバックの強さ)
	void Setup(const Math::Vector3& pos, float radius, float damage, float knockBackPower);

private:

	std::weak_ptr<Player> m_wpPlayer;

	float m_radius = 3.0f;
	float m_damage = 0.0f;
	float m_knockBackPower = 0.0f;

	bool m_hitTarget = false;

	// 当たり判定を持続させるフレーム数(1フレームだけだと判定漏れの可能性があるため少し余裕を持たせる)
	float m_lifeTime = 4.0f;
};
