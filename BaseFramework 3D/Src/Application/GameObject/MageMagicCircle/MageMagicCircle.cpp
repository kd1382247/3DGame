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
	m_telegraphTimeTotal = telegraphTime;

	m_damage = damage;

	CreateTelegraphIndicator();
	CreateMagicCircleRange();
}

void MageMagicCircle::SetEffectInfo(const std::string& fileName, float size, float speed, float startFrame, float endFrame)
{
	m_effectInfo.m_fileName = fileName;
	m_effectInfo.m_size = size;
	m_effectInfo.m_speed = speed;
	m_effectInfo.m_startFrame = startFrame;
	m_effectInfo.m_endFrame = endFrame;
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
		KdShaderManager::Instance().WriteCBColorSphereCircle(
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

void MageMagicCircle::CreateTelegraphIndicator()
{
	m_telegraphIndicatorHandle=KdShaderManager::Instance().CreateColorSphere();
}

void MageMagicCircle::UpdateTelegraphIndicator()
{
	if(m_telegraphIndicatorHandle!=-1)
	{
		// 表示する円の大きさを更新
		float progress = 1.0 - (m_telegraphTime / m_telegraphTimeTotal);
		float currentRadius = progress * m_radius;


		KdShaderManager::Instance().WriteCBColorSphereCircle(
			m_telegraphIndicatorHandle,
			GetPos(),
			currentRadius,
			Math::Vector3(2.0f,0.0f,0.0f));
	}
}

void MageMagicCircle::HideTelegraphIndicator()
{
	if(m_telegraphIndicatorHandle!=-1)
	{
		// 表示を消す
		KdShaderManager::Instance().ReleaseColorSphere(m_telegraphIndicatorHandle);

		m_telegraphIndicatorHandle = -1;
	}
}

void MageMagicCircle::Update()
{
	float deltaTime = TimeManager::Instance().GetDeltaTime();

	UpdateTelegraphIndicator();
	UpdateMagicCircleRange();

	// 予備動作中は当たり判定なし
	if (m_telegraphTime > 0.0f)
	{
		m_telegraphTime -= deltaTime;
		return;
	}
	else
	{
		HideTelegraphIndicator();
	}

	if (m_effectHandle < 0)
	{
		// エフェクト再生(最初の1回だけ呼ぶ)
		auto spEffekseerObj = KdEffekseerManager::GetInstance().Play(
			m_effectInfo.m_fileName,
			GetPos() + Math::Vector3(0.0f, 0.2f, 0.0f),
			m_effectInfo.m_size,
			m_effectInfo.m_speed,
			false,
			m_effectInfo.m_startFrame,
			m_effectInfo.m_endFrame).lock();

		if (spEffekseerObj)
		{
			m_effectHandle = spEffekseerObj->GetHandle();
			m_wpEffekseerObj = spEffekseerObj;
		}
	}
	// ハンドルベースでの終了判定(KdEffekseerObjectの生存期間に依存しない)
	else if (KdEffekseerManager::GetInstance().HasEffectFinished(m_effectHandle))
	{
		Destroy();
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
	}

	auto spEffekseerObj = m_wpEffekseerObj.lock();
	if (spEffekseerObj)
	{
		if (spEffekseerObj->GetProgress() > 0.8f)
		{
			HideMagicCircleRange();
			m_hitTarget = true;
		}
	}
}
