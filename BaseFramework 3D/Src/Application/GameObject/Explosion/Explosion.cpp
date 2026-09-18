#include "Explosion.h"

#include"../Character/Player/Player.h"
#include"../Character/AttackInfo.h"

#include"../../System/GameObjectFinder/GameObjectFinder.h"
#include"../../System/TimeManager/TimeManager.h"

void Explosion::Init()
{
	SetUpReference();
}

void Explosion::SetUpReference()
{
	m_wpPlayer = GameObjectFinder::Instance().FindObject<Player>();
}

void Explosion::Setup(const Math::Vector3& pos, float radius, float damage, float knockBackPower)
{
	SetPos(pos);

	m_radius = radius;
	m_damage = damage;
	m_knockBackPower = knockBackPower;
}

void Explosion::Update()
{
	if (!m_hitTarget)
	{
		auto spPlayer = m_wpPlayer.lock();

		if (spPlayer)
		{
			DirectX::BoundingSphere sphere;

			sphere.Center = GetPos();
			sphere.Radius = m_radius;

			KdCollider::SphereInfo sphereInfo(KdCollider::TypeBump, sphere);

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
			}

			// 当たったかどうかに関わらず、一度判定したら二度と判定しない
			m_hitTarget = true;
		}
	}

	float deltaTime = TimeManager::Instance().GetDeltaTime();

	m_lifeTime -= 60.0f * deltaTime;

	if (m_lifeTime <= 0.0f)
	{
		Destroy();
	}
}
