#include "BeholderChargeSparkBullet.h"

#include"../../../CharacterBase.h"
#include"../../Beholder/Beholder.h"
#include"../../../Player/Player.h"
#include"../../../AttackInfo.h"

#include"../../../../../System/GameObjectFinder/GameObjectFinder.h"
#include"../../../../../System/TimeManager/TimeManager.h"
#include"../../../../../System/CollisionManager/CollisionManager.h"


namespace
{

	// 弾とOwnerの距離
	constexpr float OwnerDist = 1.5f;

	// 始めの弾の大きさ
	constexpr float StartBulletSize = 0.1f;
}

void BeholderChargeSparkBullet::Init()
{
	SetUpReference();
}

void BeholderChargeSparkBullet::SetUpReference()
{
	m_wpPlayer = GameObjectFinder::Instance().FindObject<Player>();
}

void BeholderChargeSparkBullet::Setup(const BeholderChargeSparkBulletDesc& desc)
{
	m_desc       = desc;
	m_lifeTime   = m_desc.lifeTime;
}

void BeholderChargeSparkBullet::Update()
{
	// 撃ったボスがいなくなったら消える
	auto spOwner = m_wpOwner.lock();

	if (!spOwner)
	{
		Vanish();
		return;
	}

	switch (m_fireState)
	{
	case BeholderChargeSparkBullet::Preparation:

		// 発射準備の更新
		UpdatePreparation();
		break;
	case BeholderChargeSparkBullet::Standby:

		// 発射待機の更新
		UpdateStandby();
		break;
	case BeholderChargeSparkBullet::Firing:

		// 発射中の更新
		UpdateFiring();
		break;
	case BeholderChargeSparkBullet::Reflect:

		// 発射中の更新
		UpdateReflect();
		break;
	}

	// エフェクト更新
	UpdateEffect();

}

void BeholderChargeSparkBullet::ReflectSetUp()
{
	m_desc.speed = m_desc.reflectSpeed;

	// 寿命を延ばす
	m_lifeTime += m_desc.lifeTime;

	// 反射するためOwner方向のベクトルを作る
	if (auto spOwner = m_wpOwner.lock())
	{
		Math::Vector3 ownerPos = spOwner->GetBumpSphere().Center;
		m_targetDir = ownerPos - GetPos();

		// 正規化
		m_targetDir.Normalize();
	}
}

bool BeholderChargeSparkBullet::IsHitMap(const KdCollider::SphereInfo& sphereInfo)const
{
	// 壁(AABB)
	for (auto& wpObj : CollisionManager::Instance().GetObjects(CollisionLayer::AABB))
	{
		auto spObj = wpObj.lock();

		if (!spObj) { continue; }

		if (spObj->Intersects(sphereInfo, nullptr)) { return true; }
	}

	// 壁(OBB)
	for (auto& wpObj : CollisionManager::Instance().GetObjects(CollisionLayer::OBB))
	{
		auto spObj = wpObj.lock();

		if (!spObj) { continue; }

		if (spObj->Intersects(sphereInfo, nullptr)) { return true; }
	}

	return false;
}

void BeholderChargeSparkBullet::HitPlayer(const KdCollider::SphereInfo& sphereInfo)
{
	auto spPlayer = m_wpPlayer.lock();

	if (!spPlayer) { return; }

	if (!spPlayer->Intersects(sphereInfo, nullptr)) { return; }

	// ノックバックの方向を作る
	Math::Vector3 knockBackDir = spPlayer->GetPos() - GetPos();
	knockBackDir.y = 0;

	if (knockBackDir.LengthSquared() > 0.000001f)
	{
		knockBackDir.Normalize();
	}

	AttackInfo attackInfo;

	attackInfo.m_knockBackDir = knockBackDir;
	attackInfo.m_knockBackDist = m_desc.knockBackPower;
	attackInfo.m_damage = m_desc.damage;
	attackInfo.m_canReflect = true;

	spPlayer->OnHit(attackInfo);

	// パリィされたら打ち返す。そうでなければ、当たって消える
	if (spPlayer->GetParry().GetIsParrySuccess())
	{
		m_fireState = FireState::Reflect;
		ReflectSetUp();
	}
	else
	{
		Vanish();
	}
}

void BeholderChargeSparkBullet::HitOwner(const KdCollider::SphereInfo& sphereInfo)
{
	auto spOwner = std::dynamic_pointer_cast<Beholder>(m_wpOwner.lock());

	if (!spOwner) { return; }

	if (!spOwner->Intersects(sphereInfo, nullptr)) { return; }

	// ノックバックの方向を作る
	Math::Vector3 knockBackDir = spOwner->GetPos() - GetPos();
	knockBackDir.y = 0;

	if (knockBackDir.LengthSquared() > 0.000001f)
	{
		knockBackDir.Normalize();
	}

	AttackInfo attackInfo;

	attackInfo.m_knockBackDir = knockBackDir;
	attackInfo.m_knockBackDist = m_desc.knockBackPower;
	attackInfo.m_damage = m_desc.damage;
	attackInfo.m_canReflect = true;

	spOwner->OnHit(attackInfo);

	spOwner->DizzyCountUp();

	Vanish();
}

void BeholderChargeSparkBullet::PlayEffect(const Math::Vector3& pos)
{
	// IsLoop=falseで単発再生し、区間再生が終わるたびにUpdateEffect()側で再生し直す
	m_wpEffect = KdEffekseerManager::Instance().Play(
		m_desc.effect.fileName,
		pos,
		m_bulletSize,
		m_desc.effect.speed,
		false,
		m_desc.effect.startFrame,
		m_desc.effect.endFrame);
}

void BeholderChargeSparkBullet::UpdateFowardPos()
{

	Math::Vector3 pos = {};
	if (auto spOwner = m_wpOwner.lock())
	{
		pos = spOwner->GetBumpSphere().Center;
		pos += spOwner->GetMatrix().Backward() * OwnerDist;
	}

	SetPos(pos);
}

void BeholderChargeSparkBullet::UpdateBulletSize()
{
	
	float t = std::clamp( m_chargeTime / m_desc.chargeTime,0.0f,1.0f);

	// 目標の大きさに近づけていく
	m_bulletSize = std::lerp(StartBulletSize, m_desc.targetSize, t);
}

void BeholderChargeSparkBullet::UpdateEffect()
{
	auto spEffect = m_wpEffect.lock();

	if (!spEffect || !spEffect->IsPlaying())
	{
		PlayEffect(GetPos());
		spEffect = m_wpEffect.lock();
	}

	if (spEffect)
	{
		spEffect->SetPos(GetPos());
		spEffect->SetScale(m_bulletSize);
	}
}

void BeholderChargeSparkBullet::StartFiring()
{

	m_fireState = FireState::Firing;

	if (auto spPlayer = m_wpPlayer.lock())
	{
		Math::Vector3 playerPos = spPlayer->GetBumpSphere().Center;
		m_targetDir = playerPos - GetPos();

		m_targetDir.Normalize();
	}
}

void BeholderChargeSparkBullet::UpdatePreparation()
{

	float deltaTime = TimeManager::Instance().GetDeltaTime();

	m_chargeTime += deltaTime ;
	if (m_chargeTime >= m_desc.chargeTime)
	{
		// チャージが終わったら待機状態にする
		m_fireState = FireState::Standby;

		m_bulletSize = m_desc.targetSize;
	}

	UpdateFowardPos();

	UpdateBulletSize();

}

void BeholderChargeSparkBullet::UpdateStandby()
{
	UpdateFowardPos();
}

void BeholderChargeSparkBullet::UpdateFiring()
{

	UpdateMove();

	const float deltaTime = TimeManager::Instance().GetDeltaTime();

	DirectX::BoundingSphere sphere;

	sphere.Center = GetPos();
	sphere.Radius = m_desc.radius;

	KdCollider::SphereInfo sphereInfo(KdCollider::TypeBump, sphere);

	// マップ(壁)に当たったら消える
	if (IsHitMap(sphereInfo))
	{
		Vanish();
		return;
	}

	HitPlayer(sphereInfo);
}

void BeholderChargeSparkBullet::UpdateReflect()
{
	
	UpdateMove();

	const float deltaTime = TimeManager::Instance().GetDeltaTime();

	// 反射された弾の持ち主のSphereを作る
	DirectX::BoundingSphere sphere;

	sphere.Center = GetPos();
	sphere.Radius = m_desc.radius;

	KdCollider::SphereInfo sphereInfo(KdCollider::TypeBump, sphere);

	// ボスに当たるかを見る
	HitOwner(sphereInfo);
}

void BeholderChargeSparkBullet::UpdateMove()
{
	float deltaTime = TimeManager::Instance().GetDeltaTime();

	m_lifeTime -= deltaTime;
	if (m_lifeTime <= 0)
	{
		Vanish();
		return;
	}

	Math::Vector3 pos = GetPos();

	pos += m_targetDir * m_desc.speed * deltaTime*60.0f;

	SetPos(pos);

}

void BeholderChargeSparkBullet::Vanish()
{
	if (auto spEffect = m_wpEffect.lock())
	{
		spEffect->StopEffect();
	}

	KdEffekseerManager::Instance().Play("Boss/Beholder/Spark/SparkBreak.efkefc", GetPos(), 1.0f, 1.0f, false, 30, 60);

	Destroy();
}
