#pragma once

#include"BeholderSparkBulletDesc.h"

class Player;
class CharacterBase;

// Beholder専用の魔法弾
// 当たり判定・移動・エフェクト追従を担当するオブジェクト(Beholderが撃った瞬間に生成する)
class BeholderSparkBullet :public KdGameObject
{
public:

	enum FireState
	{
		Preparation, // 発射準備
		Standby,     // 発射待機
		Firing,      // 発射中
		Reflect      // 反射状態
		
	};


	BeholderSparkBullet() {}
	~BeholderSparkBullet()override {}

	void Init()override;
	void Update()override;

	void SetUpReference()override;

	// 弾の設定
	void Setup(const BeholderSparkBulletDesc& desc);

	// 弾を発射したクラスをセット
	void SetOwner(const std::shared_ptr<CharacterBase>& owner) { m_wpOwner = owner; }

	// 弾を取り消す(エフェクトを止めて、オブジェクトも消す)
	void Cancel() { Vanish(); }

	// 発射準備中か
	bool IsPreparing()const { return m_fireState == FireState::Preparation; }
	// 発射待機状態か
	bool IsStandby()const { return m_fireState == FireState::Standby; }

	// 弾を発射
	void StartFiring() { m_fireState = FireState::Firing; }

private:

	// 状態ごとの更新
	void UpdatePreparation();
	void UpdateStandby();
	void UpdateFiring();
	void UpdateReflect();
	
	// 弾を消す(エフェクト停止 + Destroy)
	void Vanish();

	// 移動
	bool Move(float deltaTime);

	// 反射した際のパラメータの変更
	void ReflectSetUp();

	// マップ(AABB/OBB)と当たっているか
	bool IsHitMap(const KdCollider::SphereInfo& sphereInfo)const;

	// プレイヤーとの当たり判定
	void HitPlayer(const KdCollider::SphereInfo& sphereInfo);

	// 反射された後の、発射したボスとの当たり判定
	void HitOwner(const KdCollider::SphereInfo& sphereInfo);


	// エフェクトを指定位置で再生する
	void PlayEffect(const Math::Vector3& pos);

	// 発射準備中のエフェクト更新
	void UpdatePreparationEffect();

	// エフェクトを弾の位置に追従させる(区間の再生が終わっていたら再生し直す)
	void UpdateEffect();
	
	// モデルのNullPointNodeの位置を更新
	void UpdateNullPointNodePos();

private:

	std::weak_ptr<Player> m_wpPlayer;

	// 弾を発射したクラス
	std::weak_ptr<CharacterBase> m_wpOwner;

	// 弾に追従させるエフェクト
	std::weak_ptr<KdEffekseerObject> m_wpEffect;

	// 発射時の設定
	BeholderSparkBulletDesc m_desc;

	// 弾の発射状態
	FireState m_fireState=FireState::Preparation;


	Math::Vector3 m_startPos   = {};
	Math::Vector3 m_endPos     = {};
	Math::Vector3 m_controlPos = {};

	// 混ぜる割合
	float m_t = 0.0f;

	// 弾の追従
	bool m_isTracking=true;

	// プレイヤーに打ち返されたか
	bool m_isReflect = false;

	bool m_isPlayEffect = false;
};
