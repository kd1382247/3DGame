#include "MageMagicSector.h"

#include<algorithm>

#include"../../../Player/Player.h"
#include"../../../AttackInfo.h"

#include"../../../../../System/GameObjectFinder/GameObjectFinder.h"
#include"../../../../../System/TimeManager/TimeManager.h"

void MageMagicSector::Init()
{
	SetUpReference();
}

void MageMagicSector::SetUpReference()
{
	m_wpPlayer = GameObjectFinder::Instance().FindObject<Player>();
}

void MageMagicSector::Setup(const MageMagicSectorDesc& desc)
{
	m_desc = desc;

	SetPos(m_desc.pos);

	// 受け取った時点で正規化
	m_desc.dir.Normalize();

	m_telegraphTime = m_desc.telegraphTime;

	// エフェクトを再生する基準座標
	m_effectPos = GetPos() + (m_desc.dir * 4.0f);

	m_telegraphIndicator.Show();
	m_rangeIndicator.Show();
}

void MageMagicSector::UpdateIndicators()
{
	// 攻撃範囲(当たり判定と同じ座標・向き・半径・角度)
	m_rangeIndicator.DrawSector(GetPos(), m_desc.dir, m_desc.radius, m_desc.angleDeg);

	// 予告は、発動に近づくにつれて範囲いっぱいまで広がっていく
	float progress = (m_desc.telegraphTime > 0.0f) ? (1.0f - (m_telegraphTime / m_desc.telegraphTime)) : 1.0f;
	progress = std::clamp(progress, 0.0f, 1.0f);

	m_telegraphIndicator.DrawSector(GetPos(), m_desc.dir, m_desc.radius * progress, m_desc.angleDeg);
}

void MageMagicSector::Cancel()
{
	// 地面に出している予告の表示を消す
	m_telegraphIndicator.Hide();
	m_rangeIndicator.Hide();

	// 再生中のエフェクトを止める
	if (auto spEffect = m_wpEffekseerObj.lock())
	{
		spEffect->StopEffect();
	}

	m_isFinished = true;

	Destroy();
}

void MageMagicSector::Update()
{
	// 取り消された後は何もしない
	if (IsExpired())
	{
		return;
	}

	float deltaTime = TimeManager::Instance().GetDeltaTime();

	UpdateIndicators();

	// 予備動作中は当たり判定なし
	if (m_telegraphTime > 0.0f)
	{
		m_telegraphTime -= deltaTime;
		return;
	}
	else
	{
		m_telegraphIndicator.Hide();
	}

	if (m_effectHandle < 0)
	{
		// エフェクト再生(最初の1回だけ呼ぶ)
		auto spEffekseerObj = KdEffekseerManager::GetInstance().Play(
			m_desc.effect.fileName,
			m_effectPos+m_desc.dir*-1.4f,
			m_desc.effect.size,
			m_desc.effect.speed,
			false,
			m_desc.effect.startFrame,
			m_desc.effect.endFrame,
			m_desc.effect.rotation).lock();

		if (spEffekseerObj)
		{
			m_effectHandle = spEffekseerObj->GetHandle();
			m_wpEffekseerObj = spEffekseerObj;
		}
	}
	// ハンドルベースでの終了判定(KdEffekseerObjectの生存期間に依存しない)
	else if (KdEffekseerManager::GetInstance().HasEffectFinished(m_effectHandle))
	{
		m_isFinished = true;
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
			
			Math::Vector3 sectorDir = m_desc.dir;
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

			float halfAngleRad = DirectX::XMConvertToRadians(m_desc.angleDeg * 0.5);
			float cosHalfAngle = cosf(halfAngleRad);

			// 距離が半径より小さい、内積がコサイン値より大きい
			if (dist<targetRadius+m_desc.radius && dot>cosHalfAngle)
			{

				AttackInfo attackInfo;
				attackInfo.m_damage = m_desc.damage;

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
			m_rangeIndicator.Hide();
			m_hitTarget = true;
		}
	}
}
