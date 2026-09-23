#include "MageMagicCircle.h"

#include"../Character/Player/Player.h"
#include"../Character/AttackInfo.h"

#include"../../System/GameObjectFinder/GameObjectFinder.h"
#include"../../System/TimeManager/TimeManager.h"

void MageMagicCircle::Init()
{
	SetUpReference();
}

void MageMagicCircle::SetUpReference()
{
	m_wpPlayer = GameObjectFinder::Instance().FindObject<Player>();
}

void MageMagicCircle::Setup(const Math::Vector3& pos, float radius, float telegraphTime, float damage)
{
	SetPos(pos);

	m_radius = radius;
	m_telegraphTime = telegraphTime;
	m_damage = damage;

	CreateMagicCircleRange();
}

void MageMagicCircle::CreateMagicCircleRange()
{
	// 攻撃範囲を表示するカラースフィアを作る
	m_colorSphereHandle = KdShaderManager::Instance().CreateColorSphere();
}

void MageMagicCircle::UpdateMagicCircleRange()
{
	if (m_colorSphereHandle != -1)
	{
		KdShaderManager::Instance().WriteCBColorSphere(
			m_colorSphereHandle,
			GetPos(),
			m_radius,
			Math::Vector3(2.0f, 0.0f, 0.0f));
	}
}

void MageMagicCircle::HideMagicCircleRange()
{
	// 表示を消す
	KdShaderManager::Instance().ReleaseColorSphere(m_colorSphereHandle);
}

void MageMagicCircle::Update()
{
	float deltaTime = TimeManager::Instance().GetDeltaTime();

	UpdateMagicCircleRange();

	// 予備動作中は当たり判定なし
	if (m_telegraphTime > 0.0f)
	{
		m_telegraphTime -= deltaTime;
		return;
	}


	if (m_effectHandle < 0)
	{
		// エフェクト再生(最初の1回だけ呼ぶ)
		auto spEffekseerObj = KdEffekseerManager::GetInstance().
			Play("Thunder/Thunder2.efkefc", GetPos() + Math::Vector3(0.0f, 0.2f, 0.0f), 0.5f, 2.0f, false,0,165).lock();

		if (spEffekseerObj)
		{
			m_effectHandle = spEffekseerObj->GetHandle();
		}
	}
	// ハンドルベースでの終了判定(KdEffekseerObjectの生存期間に依存しない)
	else if (KdEffekseerManager::GetInstance().HasEffectFinished(m_effectHandle))
	{
		Destroy();
		HideMagicCircleRange();
	}

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

				AttackInfo attackInfo;
				attackInfo.damage = m_damage;

				spPlayer->OnHit(attackInfo);


				m_hitTarget = true;
			}
		}

		// 一定時間たつと当たり判定しない
		m_hitboxActiveTime -= deltaTime;
		if (m_hitboxActiveTime < 0)
		{
			m_hitTarget = true;
		}
	}
}
