#include "EnergyBullet.h"

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
	float deltaTime = TimeManager::Instance().GetDeltaTime();

	// 移動
	Math::Vector3 move = m_dir * (m_speed * 60.0f) * deltaTime;
	SetPos(GetPos() + move);

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

	if (!m_hitTarget)
	{
		auto spPlayer = m_wpPlayer.lock();

		if (spPlayer)
		{
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

				attackInfo.knockBackDir = knockBackDir;
				attackInfo.knockBackPower = m_knockBackPower;
				attackInfo.damage = m_damage;

				spPlayer->OnHit(attackInfo);

				m_hitTarget = true;

				// 命中したので消える
				Vanish();
				return;
			}
		}
	}

	// 一定時間で消える
	m_lifeTime -= 60* deltaTime;

	if (m_lifeTime <= 0.0f)
	{
		Vanish();
	}
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

void EnergyBullet::PlayEnergyEffect(const Math::Vector3& pos)
{
	// IsLoop=falseで単発再生し、区間再生が終わるたびにUpdate()側で再生し直す
	m_wpEffect = KdEffekseerManager::GetInstance().
		Play("Energy/Energy.efkefc", pos, 0.3f, 1.0f, false, 10, 50);
}

void EnergyBullet::Vanish()
{
	if (auto spEffect = m_wpEffect.lock())
	{
		spEffect->StopEffect();
	}

	Destroy();
}
