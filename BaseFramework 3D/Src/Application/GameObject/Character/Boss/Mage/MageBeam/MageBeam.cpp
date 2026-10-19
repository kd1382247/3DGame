#include "MageBeam.h"

#include"../../../Player/Player.h"
#include"../../../AttackInfo.h"

#include"../../../../../System/GameObjectFinder/GameObjectFinder.h"
#include"../../../../../System/TimeManager/TimeManager.h"

void MageBeam::Init()
{
	SetUpReference();
}

void MageBeam::SetUpReference()
{
	m_wpPlayer = GameObjectFinder::Instance().FindObject<Player>();
}

void MageBeam::Setup(const MageBeamDesc& desc)
{
	m_desc = desc;

	SetPos(m_desc.pos);
	m_desc.dir.Normalize();

	// 生成直後からすぐ判定できる状態にしておく
	m_hitCooldown = 0.0f;

	m_remainingTime = m_desc.duration;
}



void MageBeam::Cancel()
{
	// 再生中のエフェクトを止める
	if (auto spEffect = m_wpEffekseerObj.lock())
	{
		spEffect->StopEffect();
	}

	m_isFinished = true;

	Destroy();
}

void MageBeam::Update()
{
	// 取り消された後は何もしない(エフェクトをもう一度再生しないようにする)
	if (IsExpired())
	{
		return;
	}

	float deltaTime = TimeManager::Instance().GetDeltaTime();

	// 持続時間が切れたら、エフェクトを止めて消える
	m_remainingTime -= deltaTime;

	if (m_remainingTime <= 0.0f)
	{
		Cancel();

		return;
	}

	if (m_effectHandle < 0)
	{
		// エフェクト再生(最初の1回だけ呼ぶ)
		auto spEffekseerObj = KdEffekseerManager::GetInstance().Play(
			m_desc.effect.fileName,
			m_desc.effect.effectPos,
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

	if (m_hitCooldown > 0.0f)
	{
		m_hitCooldown -= deltaTime;
	}

	auto spPlayer = m_wpPlayer.lock();

	if (spPlayer && m_hitCooldown <= 0.0f)
	{
		// ビームの前方軸・右軸(横方向)をローカル軸として作る(水平面のみで判定)
		Math::Vector3 forward = m_desc.dir;
		forward.y = 0.0f;
		forward.Normalize();

		Math::Vector3 right = Math::Vector3::Up.Cross(forward);
		right.Normalize();

		float targetRadius = spPlayer->GetBumpSphere().Radius;

		Math::Vector3 toTarget = spPlayer->GetPos() - GetPos();
		toTarget.y = 0.0f;

		// 前方方向への距離・横方向のズレに分解する
		float f = toTarget.Dot(forward);
		float r = toTarget.Dot(right);

		// カラースフィア(矩形)はPosを中心に前後・左右へRectSize/2ずつ広がる作りなので、それに合わせる
		bool inLength = (f > -(m_desc.length * 0.5f + targetRadius)) && (f < (m_desc.length * 0.5f + targetRadius));
		bool inWidth = (r > -(m_desc.width * 0.5f + targetRadius)) && (r < (m_desc.width * 0.5f + targetRadius));

		if (inLength && inWidth)
		{
			AttackInfo attackInfo;
			attackInfo.m_damage = m_desc.damage;

			spPlayer->OnHit(attackInfo);

			m_hitCooldown = m_desc.hitInterval;
		}
	}
}
