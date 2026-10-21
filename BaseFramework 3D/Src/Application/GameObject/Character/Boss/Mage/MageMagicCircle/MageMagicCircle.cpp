#include "MageMagicCircle.h"

#include<algorithm>

#include"../../../Player/Player.h"
#include"../../../AttackInfo.h"

#include"../../../../../System/GameObjectFinder/GameObjectFinder.h"
#include"../../../../../System/TimeManager/TimeManager.h"

void MageMagicCircle::Init()
{
	SetUpReference();
}

void MageMagicCircle::SetUpReference()
{
	m_wpPlayer = GameObjectFinder::Instance().FindObject<Player>();
}

void MageMagicCircle::Setup(const MageMagicCircleDesc& desc)
{
	m_desc = desc;

	SetPos(m_desc.pos);

	m_telegraphTime = m_desc.telegraphTime;
	m_hitDelayTime = m_desc.hitDelay;

	m_telegraphIndicator.Show();
	m_rangeIndicator.Show();
}

void MageMagicCircle::UpdateIndicators()
{
	// 攻撃範囲(当たり判定と同じ座標・半径)
	m_rangeIndicator.DrawCircle(GetPos(), m_desc.radius);

	// 予告は、発動に近づくにつれて範囲いっぱいまで広がっていく
	float progress = (m_desc.telegraphTime > 0.0f) ? (1.0f - (m_telegraphTime / m_desc.telegraphTime)) : 1.0f;
	progress = std::clamp(progress, 0.0f, 1.0f);

	m_telegraphIndicator.DrawCircle(GetPos(), m_desc.radius * progress);
}

void MageMagicCircle::Cancel()
{
	// 地面に出している予告の表示を消す
	m_telegraphIndicator.Hide();
	m_rangeIndicator.Hide();

	// 再生中のエフェクトを止める
	if (auto spEffect = m_wpEffekseerObj.lock())
	{
		spEffect->StopEffect();
	}
	// エフェクトの管理側から外れていても、まだ再生中なら、ハンドルで直接止める
	else if (m_effectHandle >= 0 && !KdEffekseerManager::GetInstance().HasEffectFinished(m_effectHandle))
	{
		KdEffekseerManager::GetInstance().StopEffect(m_effectHandle);
	}

	Destroy();
}

void MageMagicCircle::Update()
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
			GetPos() + Math::Vector3(0.0f, 0.2f, 0.0f),
			m_desc.effect.size,
			m_desc.effect.speed,
			false,
			m_desc.effect.startFrame,
			m_desc.effect.endFrame).lock();

		if (spEffekseerObj)
		{
			m_effectHandle = spEffekseerObj->GetHandle();
			m_wpEffekseerObj = spEffekseerObj;
		}

		// エフェクトが出るのに合わせて、SEを1回だけ鳴らす(パスが指定されているときだけ)
		if (!m_isPlaySE && !m_desc.sePath.empty())
		{
			KdAudioManager::Instance().PlaySE(m_desc.sePath.c_str());
			m_isPlaySE = true;
		}
	}
	// ハンドルベースでの終了判定(KdEffekseerObjectの生存期間に依存しない)
	else if (KdEffekseerManager::GetInstance().HasEffectFinished(m_effectHandle))
	{
		m_telegraphIndicator.Hide();
		m_rangeIndicator.Hide();
		Destroy();
		return;
	}

	// エフェクトの進行度が終了値に達したら、表示円を消して、以降は当たり判定をしない
	auto spEffekseerObj = m_wpEffekseerObj.lock();

	if (spEffekseerObj && spEffekseerObj->GetProgress() >= m_desc.hitEndProgress)
	{
		m_telegraphIndicator.Hide();
		m_rangeIndicator.Hide();
		m_hitTarget = true;
	}

	// エフェクトが出てから、当たり判定が出るまで待つ
	if (m_hitDelayTime > 0.0f)
	{
		m_hitDelayTime -= deltaTime;
	}

	if (!m_hitTarget && m_hitDelayTime <= 0.0f)
	{
		auto spPlayer = m_wpPlayer.lock();

		if (spPlayer)
		{
			DirectX::BoundingSphere sphere;

			sphere.Center = GetPos();
			sphere.Radius = m_desc.radius;

			KdCollider::SphereInfo sphereInfo(KdCollider::TypeBump, sphere);

			if (spPlayer->Intersects(sphereInfo, nullptr))
			{

				AttackInfo attackInfo;
				attackInfo.m_damage = m_desc.damage;

				spPlayer->OnHit(attackInfo);

				m_hitTarget = true;
			}
		}
	}
}
