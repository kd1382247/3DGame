#include "MageBeam.h"

#include"../Character/Player/Player.h"
#include"../Character/AttackInfo.h"

#include"../../System/GameObjectFinder/GameObjectFinder.h"
#include"../../System/TimeManager/TimeManager.h"

namespace
{
	// 再ヒットまでの間隔(秒)
	constexpr float kHitInterval = 1.0f;
}

void MageBeam::Init()
{
	SetUpReference();
}

void MageBeam::SetUpReference()
{
	m_wpPlayer = GameObjectFinder::Instance().FindObject<Player>();
}

void MageBeam::Setup(const Math::Vector3& pos, const Math::Vector3& dir, float length, float width, float damage, float duration)
{
	SetPos(pos);

	m_beam.m_dir = dir;

	// 受け取った時点で正規化
	m_beam.m_dir.Normalize();

	m_beam.m_length = length;
	m_beam.m_width = width;

	m_damage = damage;
	m_duration = duration;

	// 生成直後からすぐ判定できる状態にしておく
	m_hitCooldown = 0.0f;
}

void MageBeam::SetEffectInfo(const std::string& fileName, float size, float speed, float startFrame, float endFrame, const Math::Vector3& rotation, const Math::Vector3& pos)
{
	m_effectInfo.m_fileName = fileName;
	m_effectInfo.m_pos = pos;
	m_effectInfo.m_size = size;
	m_effectInfo.m_speed = speed;
	m_effectInfo.m_startFrame = startFrame;
	m_effectInfo.m_endFrame = endFrame;
	m_effectInfo.m_rotation = rotation;
}


void MageBeam::Update()
{
	float deltaTime = TimeManager::Instance().GetDeltaTime();

	if (m_effectHandle < 0)
	{
		// エフェクト再生(最初の1回だけ呼ぶ)
		auto spEffekseerObj = KdEffekseerManager::GetInstance().Play(
			m_effectInfo.m_fileName,
			m_effectInfo.m_pos,
			m_effectInfo.m_size,
			m_effectInfo.m_speed,
			false,
			m_effectInfo.m_startFrame,
			m_effectInfo.m_endFrame,
			m_effectInfo.m_rotation).lock();

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
		Math::Vector3 forward = m_beam.m_dir;
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
		bool inLength = (f > -(m_beam.m_length * 0.5f + targetRadius)) && (f < (m_beam.m_length * 0.5f + targetRadius));
		bool inWidth = (r > -(m_beam.m_width * 0.5f + targetRadius)) && (r < (m_beam.m_width * 0.5f + targetRadius));

		if (inLength && inWidth)
		{
			AttackInfo attackInfo;
			attackInfo.damage = m_damage;

			spPlayer->OnHit(attackInfo);

			m_hitCooldown = kHitInterval;
		}
	}
}
