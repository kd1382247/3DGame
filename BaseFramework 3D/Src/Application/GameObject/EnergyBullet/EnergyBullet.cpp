#include "EnergyBullet.h"

#include"../Character/CharacterBase.h"
#include"../Character/Player/Player.h"
#include"../Character/AttackInfo.h"

#include"../../System/GameObjectFinder/GameObjectFinder.h"
#include"../../System/TimeManager/TimeManager.h"
#include"../../System/CollisionManager/CollisionManager.h"

void EnergyBullet::Init()
{
	SetUpReference();
}

void EnergyBullet::SetUpReference()
{
	m_wpPlayer = GameObjectFinder::Instance().FindObject<Player>();
}

void EnergyBullet::Setup(const Math::Vector3& pos, const Math::Vector3& dir, float speed, float radius,
	float damage, float knockBackPower, float lifeTime)
{
	SetPos(pos);

	m_dir = dir;
	m_speed = speed;
	m_radius = radius;
	m_damage = damage;
	m_knockBackPower = knockBackPower;

	// 秒数をフレーム換算で保持
	m_lifeTime = lifeTime * 60.0f;

}

void EnergyBullet::Update()
{
	auto spOwner = m_wpOwner.lock();
	if (!spOwner)
	{
		Vanish();
		return;
	}

	float deltaTime = TimeManager::Instance().GetDeltaTime();

	// 移動
	Math::Vector3 move = m_dir * (m_speed * 60.0f) * deltaTime;
	SetPos(GetPos() + move);

	UpdateEffect();

	DirectX::BoundingSphere sphere;

	sphere.Center = GetPos();
	sphere.Radius = m_radius;

	KdCollider::SphereInfo sphereInfo(KdCollider::TypeBump, sphere);

	// マップ(壁)に当たったら消える
	if (IsHitMap(sphereInfo))
	{
		Vanish();
		return;
	}

	// 一定時間で消える
	m_lifeTime -= 60 * deltaTime;

	if (m_lifeTime <= 0.0f)
	{
		Vanish();
		return;
	}

	// 反射されたらPlayerとの判定をしない
	if (m_isReflect)
	{
		IsOwnerHit(sphereInfo);
		return;
	}

	IsPlayerHit(sphereInfo);
	
}

bool EnergyBullet::IsHitMap(const KdCollider::SphereInfo& sphereInfo)const
{
	// 壁(AABB)
	for (auto& wpObj : CollisionManager::Instance().GetObjects(CollisionLayer::AABB))
	{
		auto spObj = wpObj.lock();

		if (!spObj)
		{
			continue;
		}

		if (spObj->Intersects(sphereInfo, nullptr))
		{
			return true;
		}
	}

	// 壁(OBB)
	for (auto& wpObj : CollisionManager::Instance().GetObjects(CollisionLayer::OBB))
	{
		auto spObj = wpObj.lock();

		if (!spObj)
		{
			continue;
		}

		if (spObj->Intersects(sphereInfo, nullptr))
		{
			return true;
		}
	}

	return false;
}

void EnergyBullet::IsPlayerHit(const KdCollider::SphereInfo& sphereInfo)
{
	auto spPlayer = m_wpPlayer.lock();

	if (!spPlayer)
	{
		return;
	}

	if (spPlayer->Intersects(sphereInfo, nullptr))
	{
		// ノックバックの方向を作る
		Math::Vector3 knockBackDir = spPlayer->GetPos() - GetPos();
		knockBackDir.y = 0;

		if (knockBackDir.LengthSquared() > 0.000001f)
		{
			knockBackDir.Normalize();
		}

		AttackInfo attackInfo;

		attackInfo.m_knockBackDir = knockBackDir;
		attackInfo.m_knockBackPower = m_knockBackPower;
		attackInfo.m_damage = m_damage;
		attackInfo.m_canReflect = true;

		spPlayer->OnHit(attackInfo);

		if (spPlayer->GetParry().GetIsParrySuccess())
		{
			

			m_isReflect = true;
			m_lifeTime += m_lifeTimeDuration;

			m_speed += 0.1f;
			auto spOwner = m_wpOwner.lock();
			if(spOwner)
			{
				m_dir = (spOwner->GetPos()+Math::Vector3(0.0f,0.5f,0.0f)) - GetPos();
				m_dir.Normalize();
			}
		}
		else
		{
			Vanish();
		}

	}
}

void EnergyBullet::IsOwnerHit(const KdCollider::SphereInfo& sphereInfo)
{
	auto spOwner = m_wpOwner.lock();

	if (!spOwner)
	{
		return;
	}
	if (spOwner->Intersects(sphereInfo, nullptr))
	{
		// ノックバックの方向を作る
		Math::Vector3 knockBackDir = spOwner->GetPos() - GetPos();
		knockBackDir.y = 0;

		if (knockBackDir.LengthSquared() > 0.000001f)
		{
			knockBackDir.Normalize();
		}

		AttackInfo attackInfo;

		attackInfo.m_knockBackDir = knockBackDir;
		attackInfo.m_knockBackPower = m_knockBackPower;
		attackInfo.m_damage = m_damage;
		attackInfo.m_canReflect = true;

		spOwner->OnHit(attackInfo);
		Vanish();
	}
}

void EnergyBullet::PlayEnergyEffect(const Math::Vector3& pos)
{
	// IsLoop=falseで単発再生し、区間再生が終わるたびにUpdate()側で再生し直す
	m_wpEffect = KdEffekseerManager::GetInstance().
		Play("Energy/Energy.efkefc", pos, 0.3f, 1.0f, false, 10, 50);
}

void EnergyBullet::UpdateEffect()
{
	// エフェクトを弾の位置に追従させる
	// (区間の再生が終わっている場合は、こちらで再生し直してループさせる)
	auto spEffect = m_wpEffect.lock();

	if (!spEffect || !spEffect->IsPlaying())
	{
		PlayEnergyEffect(GetPos());
		spEffect = m_wpEffect.lock();
	}

	if (spEffect)
	{
		spEffect->SetPos(GetPos());
	}
}

void EnergyBullet::Vanish()
{
	if (auto spEffect = m_wpEffect.lock())
	{
		spEffect->StopEffect();
	}

	Destroy();
}
