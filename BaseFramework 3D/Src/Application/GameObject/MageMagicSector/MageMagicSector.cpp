#include "MageMagicSector.h"

#include"../Character/Player/Player.h"
#include"../Character/AttackInfo.h"

#include"../../System/GameObjectFinder/GameObjectFinder.h"
#include"../../System/TimeManager/TimeManager.h"

void MageMagicSector::Init()
{
	SetUpReference();
}

void MageMagicSector::SetUpReference()
{
	m_wpPlayer = GameObjectFinder::Instance().FindObject<Player>();
}

void MageMagicSector::Setup(const Math::Vector3& pos,const Math::Vector3& dir, float angleDeg, float radius, float telegraphTime, float damage)
{
	SetPos(pos);

	m_sector.dir = dir;

	// 受け取った時点で正規化
	m_sector.dir.Normalize();

	m_sector.m_angleDeg = angleDeg;
	m_sector.m_radius = radius;

	m_telegraphTime = telegraphTime;
	m_telegraphTimeTotal = telegraphTime;

	m_damage = damage;

	CreateTelegraphIndicator();
	CreateMagicSectorRange();
}

void MageMagicSector::SetEffectInfo(const std::string& fileName, float size, float speed, float startFrame, float endFrame)
{
	m_effectInfo.m_fileName = fileName;
	m_effectInfo.m_size = size;
	m_effectInfo.m_speed = speed;
	m_effectInfo.m_startFrame = startFrame;
	m_effectInfo.m_endFrame = endFrame;

	// 座標をセット
	m_effectInfo.m_pos = GetPos() + (m_sector.dir * 4.0f);
}


void MageMagicSector::CreateMagicSectorRange()
{
	// 攻撃範囲を表示するカラースフィアを作る
	m_colorSphereHandle = KdShaderManager::Instance().CreateColorSphere();
}

void MageMagicSector::UpdateMagicSectorRange()
{
	if (m_colorSphereHandle != -1)
	{
		KdShaderManager::Instance().WriteCBColorSphereSector(
			m_colorSphereHandle,
			GetPos(),
			m_mWorld.Backward(),
			m_sector.m_radius,
			m_sector.m_angleDeg,
			Math::Vector3(2.0f, 0.0f, 0.0f));
	}
}

void MageMagicSector::HideMagicSectorRange()
{
	// 表示を消す
	KdShaderManager::Instance().ReleaseColorSphere(m_colorSphereHandle);
}


void MageMagicSector::CreateTelegraphIndicator()
{
	m_telegraphIndicatorHandle = KdShaderManager::Instance().CreateColorSphere();
}

void MageMagicSector::UpdateTelegraphIndicator()
{
	if (m_telegraphIndicatorHandle != -1)
	{
		// 表示する円の大きさを更新
		float progress = 1.0 - (m_telegraphTime / m_telegraphTimeTotal);
		float currentRadius = progress * m_sector.m_radius;

		KdShaderManager::Instance().WriteCBColorSphereSector(
			m_telegraphIndicatorHandle,
			GetPos(),
			m_mWorld.Backward(),
			currentRadius,
			m_sector.m_angleDeg,
			Math::Vector3(2.0f, 0.0f, 0.0f));
	}
}

void MageMagicSector::HideTelegraphIndicator()
{
	if (m_telegraphIndicatorHandle != -1)
	{
		// 表示を消す
		KdShaderManager::Instance().ReleaseColorSphere(m_telegraphIndicatorHandle);

		m_telegraphIndicatorHandle = -1;
	}
}


void MageMagicSector::Update()
{
	float deltaTime = TimeManager::Instance().GetDeltaTime();

	UpdateTelegraphIndicator();
	UpdateMagicSectorRange();

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
			m_effectInfo.m_pos+m_mWorld.Backward()*-3.8f,
			m_effectInfo.m_size,
			m_effectInfo.m_speed,
			false,
			m_effectInfo.m_startFrame,
			m_effectInfo.m_endFrame,
			Math::Vector3(0,145,0)).lock();

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
			// 半径だけ受け取る
			float targetRadius = spPlayer->GetBumpSphere().Radius;

			Math::Vector3 targetPos = spPlayer->GetPos();

			Math::Vector3 toTargegtDir = targetPos - GetPos();
			toTargegtDir.y = 0;
			
			Math::Vector3 sectorDir = m_sector.dir;
			sectorDir.y = 0;

			float dist = toTargegtDir.Length();

			if (dist < 0.000001f)
			{
				m_hitTarget = true;
				return;
			}

			// 正規化
			toTargegtDir.Normalize();

			float dot = toTargegtDir.Dot(sectorDir);

			float halfAngleRad = DirectX::XMConvertToRadians(m_sector.m_angleDeg * 0.5);
			float cosHalfAngle = cosf(halfAngleRad);

			// 距離が半径より小さい、内積がコサイン値より大きい
			if (dist<targetRadius+m_sector.m_radius && dot>cosHalfAngle)
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
		if (spEffekseerObj->GetProgress() > 0.6f)
		{
			HideMagicSectorRange();
			m_hitTarget = true;
		}
	}
}
