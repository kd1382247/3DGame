#pragma once

class Player;

// Energy弾の当たり判定・移動・エフェクト追従を担当するオブジェクト
// (StarFishなどの遠隔攻撃が、発射した瞬間に生成する)
class EnergyBullet :public KdGameObject
{
public:

	EnergyBullet() {}
	~EnergyBullet()override {}

	void Init()override;
	void Update()override;

	void SetUpReference()override;

	// 弾の設定(座標・進行方向・速度・半径・ダメージ・ノックバックの強さ・寿命(秒))
	void Setup(const Math::Vector3& pos, const Math::Vector3& dir, float speed, float radius,
		float damage, float knockBackPower, float lifeTime);

private:

	// 弾を消す(エフェクト停止 + Destroy)
	void Vanish();

	// マップ(AABB/OBB)と当たっているか
	bool IsHitMap(const KdCollider::SphereInfo& sphereInfo)const;

	// Energyエフェクトを指定位置で(再)生させる
	// (KdEffekseerManagerのIsLoop=trueは再生の度に別インスタンスを内部生成してしまい、
	//  外部から止められなくなるため、ループはこちらで管理する)
	void PlayEnergyEffect(const Math::Vector3& pos);

	std::weak_ptr<Player> m_wpPlayer;

	// 弾に追従させるEnergyエフェクト
	std::weak_ptr<KdEffekseerObject> m_wpEffect;

	// 進行方向(発射時に決定、以後は変わらない)
	Math::Vector3 m_dir = Math::Vector3::Zero;

	float m_speed = 1.0f;
	float m_radius = 1.5f;
	float m_damage = 0.0f;
	float m_knockBackPower = 0.0f;

	bool m_hitTarget = false;

	// 消滅までの残りフレーム数(60fps換算)
	float m_lifeTime = 300.0f;
};
