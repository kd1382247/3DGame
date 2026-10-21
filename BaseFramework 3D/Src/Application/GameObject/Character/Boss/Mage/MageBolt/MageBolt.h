#pragma once

#include"MageBoltDesc.h"

class Player;
class CharacterBase;

// Mage専用の魔法弾
// 当たり判定・移動・エフェクト追従を担当するオブジェクト(Mageが撃った瞬間に生成する)
class MageBolt :public KdGameObject
{
public:

	MageBolt() {}
	~MageBolt()override {}

	void Init()override;
	void Update()override;

	void SetUpReference()override;

	// 弾の設定
	void Setup(const MageBoltDesc& desc);

	// 弾を発射したクラスをセット
	void SetOwner(const std::shared_ptr<CharacterBase>& owner) { m_wpOwner = owner; }

	// 弾を取り消す(エフェクトを止めて、オブジェクトも消す)
	void Cancel() { Vanish(); }

private:

	// 弾を消す(エフェクト停止 + Destroy)
	void Vanish();

	// 移動
	void Move(float deltaTime);

	// マップ(AABB/OBB)と当たっているか
	bool IsHitMap(const KdCollider::SphereInfo& sphereInfo)const;

	// プレイヤーとの当たり判定
	void HitPlayer(const KdCollider::SphereInfo& sphereInfo);

	// 反射された後の、発射したボスとの当たり判定
	void HitOwner(const KdCollider::SphereInfo& sphereInfo);

	// パリィされたとき:弾をボスへ打ち返す
	void Reflect();

	// エフェクトを指定位置で再生する
	// (KdEffekseerManagerのIsLoop=trueは再生の度に別インスタンスを内部生成してしまい、
	//  外部から止められなくなるため、ループはこちらで管理する)
	void PlayEffect(const Math::Vector3& pos);

	// エフェクトを弾の位置に追従させる(区間の再生が終わっていたら再生し直す)
	void UpdateEffect();

private:

	std::weak_ptr<Player> m_wpPlayer;

	// 弾を発射したクラス
	std::weak_ptr<CharacterBase> m_wpOwner;

	// 弾に追従させるエフェクト
	std::weak_ptr<KdEffekseerObject> m_wpEffect;

	// 発射時の設定
	MageBoltDesc m_desc;

	// 進行方向(反射されたときに変わる)
	Math::Vector3 m_dir = Math::Vector3::Zero;

	// [m/F] 今の速さ(反射されたときに上がる)
	float m_speed = 0.0f;

	// [秒] 消えるまでの残り時間
	float m_lifeTime = 0.0f;

	// プレイヤーに打ち返されたか
	bool m_isReflect = false;

};
