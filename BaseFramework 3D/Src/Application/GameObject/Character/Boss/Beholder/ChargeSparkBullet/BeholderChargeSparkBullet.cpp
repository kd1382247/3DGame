#include "BeholderChargeSparkBullet.h"

#include"../../../CharacterBase.h"
#include"../../../Player/Player.h"
#include"../../../AttackInfo.h"

#include"../../../../../System/GameObjectFinder/GameObjectFinder.h"
#include"../../../../../System/TimeManager/TimeManager.h"
#include"../../../../../System/CollisionManager/CollisionManager.h"

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
	m_desc = desc;
	m_lifeTime = m_desc.lifeTime;
	SetPos(desc.pos);
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
	attackInfo.m_knockBackPower = m_desc.knockBackPower;
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
	auto spOwner = m_wpOwner.lock();

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
	attackInfo.m_knockBackPower = m_desc.knockBackPower;
	attackInfo.m_damage = m_desc.damage;
	attackInfo.m_canReflect = true;

	spOwner->OnHit(attackInfo);

	Vanish();
}

void BeholderChargeSparkBullet::PlayEffect(const Math::Vector3& pos)
{
	// IsLoop=falseで単発再生し、区間再生が終わるたびにUpdateEffect()側で再生し直す
	m_wpEffect = KdEffekseerManager::GetInstance().Play(
		m_desc.effect.fileName,
		pos,
		m_desc.effect.size,
		m_desc.effect.speed,
		false,
		m_desc.effect.startFrame,
		m_desc.effect.endFrame);
}

void BeholderChargeSparkBullet::UpdatePreparationEffect()
{
	// 一度だけエフェクト再生を再生
	if (!m_isPlayEffect)
	{
		auto spEffect = m_wpEffect.lock();
		if (!spEffect || !spEffect->IsPlaying())
		{
			m_wpEffect = KdEffekseerManager::GetInstance().Play(
				m_desc.effect.fileName,
				GetPos(),
				m_desc.effect.size,
				0.2f,
				false,
				0, 10);

			m_isPlayEffect = true;
		}
	}
	else
	{
		// エフェクトの再生が終わったら待機状態にする
		auto spEffect = m_wpEffect.lock();

		if (!spEffect || !spEffect->IsPlaying())
		{
			PlayEffect(GetPos());
			m_fireState = FireState::Standby;
		}
		else
		{
			spEffect->SetPos(GetPos());
		}
	}
}

void BeholderChargeSparkBullet::UpdateFowardPos()
{
	auto spOwner = m_wpOwner.lock();
	if (!spOwner)
	{
		return;
	}

	Math::Matrix worldMat = GetMatrix() * spOwner->GetMatrix();

	SetPos(worldMat.Translation());
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
	}
}

void BeholderChargeSparkBullet::StartFiring()
{

	m_fireState = FireState::Firing;

	if (auto spPlayer = m_wpPlayer.lock())
	{
		Math::Vector3 playerPos = spPlayer->GetBumpSphere().Center;
		m_targetDir = playerPos - GetPos();
	}
}

void BeholderChargeSparkBullet::UpdatePreparation()
{

	UpdateFowardPos();

	// エフェクト更新
	UpdatePreparationEffect();
}

void BeholderChargeSparkBullet::UpdateStandby()
{
	UpdateFowardPos();

	// 終着地点
	if (auto spPlayer = m_wpPlayer.lock())
	{
		Math::Vector3 playerPos = spPlayer->GetPos();
		Math::Vector3 offsetPos = {};

		offsetPos.y = spPlayer->GetBumpSphereOffsetY();
	}

	UpdateEffect();
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

	UpdateEffect();
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

	UpdateEffect();
}

void BeholderChargeSparkBullet::UpdateMove()
{
	float deltaTime = TimeManager::Instance().GetDeltaTime();

	m_lifeTime -= deltaTime * 60.0f;
	if (m_lifeTime <= 0)
	{
		Vanish();
		return;
	}

	Math::Vector3 pos = GetPos();

	pos += m_targetDir * m_desc.speed * deltaTime;

	SetPos(pos);

}

void BeholderChargeSparkBullet::Vanish()
{
	if (auto spEffect = m_wpEffect.lock())
	{
		spEffect->StopEffect();
	}

	KdEffekseerManager::GetInstance().Play("Boss/Beholder/Spark/SparkBreak.efkefc", GetPos(), 0.6, 1.0f, false, 30, 60);

	Destroy();
}
