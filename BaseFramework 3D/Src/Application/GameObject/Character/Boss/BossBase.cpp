#include "BossBase.h"

#include"../../../System/GameObjectFinder/GameObjectFinder.h"

#include"../Player/Player.h"

#include"../../Camera/TPSCamera/TPSCamera.h"

#include"../../FlyText/FlyTextManager.h"

void BossBase::Init()
{
	// カテゴリーをセット
	SetObjectCategory(ObjectCategory::Character);
	m_bumpPushRate = 0.0f;
}

void BossBase::PostUpdate()
{
	CharacterBase::PostUpdate();
}

void BossBase::SetUpReference()
{
	m_wpPlayer = GameObjectFinder::Instance().FindObject<Player>();

	m_wpCamera = GameObjectFinder::Instance().FindObject<TPSCamera>();
}

void BossBase::DrawInspector()
{
	DrawParameterInspector();
}

void BossBase::OnHit(const AttackInfo attackInfo)
{
	m_health.TakeDamage(attackInfo.m_damage);

	FlyTextManager::Instance().CreateDamateText(attackInfo.m_damage, GetPos(),m_flyTextPath);

	PlayHitEffect();
}

void BossBase::UpdateMove()
{
	
	auto spPlayer = m_wpPlayer.lock();

	if (!spPlayer)
	{
		return;
	}

	Math::Vector3 targetDir = spPlayer->GetPos() - GetPos();

	// X・Z平面だけで移動・到着判定する
	targetDir.y = 0.0f;

	SetMoveDir(targetDir);

	float distance = targetDir.Length();


	if (m_hasReachedTarget)
	{
		if (distance > m_reachDistanceMargin)
		{
			m_hasReachedTarget = false;
			PlayWalkAnimation();
		}
		else
		{
			PlayIdleAnimation();
			return;
		}
	}
	else
	{
		if (distance < m_reachDistance)
		{
			m_hasReachedTarget = true;
			PlayIdleAnimation();
			return;
		}
		else
		{
			PlayWalkAnimation();
		}
	}
	

	float         moveSpeed = GetMoveSpeed();

	if (distance < moveSpeed)
	{
		moveSpeed = distance;
	}

	targetDir.Normalize();


	Math::Vector3 move = targetDir * (moveSpeed * 60.0f) * m_deltaTime;

	AddPendingMove(move);
}

void BossBase::SetTargetDir()
{

	auto spPlayer = m_wpPlayer.lock();

	if (!spPlayer)
	{
		return;
	}

	Math::Vector3 targetDir = spPlayer->GetPos() - GetPos();

	// X・Z平面だけで移動・到着判定する
	targetDir.y = 0.0f;

	SetMoveDir(targetDir);
}

void BossBase::StartCameraIntro()
{
	auto spCamera = m_wpCamera.lock();

	if (!spCamera)
	{
		return;
	}

	TPSCamera::BossIntroParam param;
	spCamera->StartBossIntro(GetPos(), param);
}

void BossBase::StartCameraRoar()
{
	auto spCamera = m_wpCamera.lock();

	if (!spCamera)
	{
		return;
	}

	spCamera->StartRoar();
}

bool BossBase::IsCameraIntroHold()
{
	auto spCamera = m_wpCamera.lock();

	if (!spCamera)
	{
		return true; 
	}
	
	return spCamera->IsBossIntroHold();

}

bool BossBase::IsBossIntroPlaying()
{

	auto spCamera = m_wpCamera.lock();

	if (!spCamera)
	{
		return false;
	}

	return spCamera->IsBossIntroPlaying();
}


bool BossBase::IsSecondPhase()const
{
	return m_health.GetCurrentHP() <= m_health.GetMaxHP() / 2;
}

int BossBase::LotteryPattern(const std::vector<float>& weights)const
{
	float total = 0.0f;

	for (const float weight : weights)
	{
		if (weight > 0.0f)
		{
			total += weight;
		}
	}

	if (total <= 0.0f)
	{
		return -1;
	}

	float lot = KdRandom::GetFloat(0.0f, total);

	float sum = 0.0f;

	for (size_t i = 0; i < weights.size(); i++)
	{
		if (weights[i] <= 0.0f)
		{
			continue;
		}

		sum += weights[i];

		if (lot <= sum)
		{
			return static_cast<int>(i);
		}
	}

	return -1;
}

void BossBase::PlayHitEffect()
{

	auto spPlayer = m_wpPlayer.lock();

	Math::Vector3 playerPos = {};

	if (spPlayer)
	{
		playerPos = spPlayer->GetPos();
	}

	Math::Vector3 bossDir = playerPos - GetPos();

	bossDir.y = 0.0f;
	if (bossDir.LengthSquared() <= 0.000001f)
	{
		bossDir = Math::Vector3::Backward;
	}
	bossDir.Normalize();

	// ボスから見てプレイヤーの方向に、エフェクトの位置を持っていく
	Math::Vector3 effectPos = GetPos() + bossDir * 1.2f+Math::Vector3(0.0f, 0.8f, 0.0f);


	KdEffekseerManager::GetInstance().
		Play("Hit/Hit2.efkefc",effectPos, 0.3f, 0.8f, false);

	KdEffekseerManager::GetInstance().
		Play("Hit/Hit3.efkefc", effectPos, 0.3f, 0.6f, false);

	KdEffekseerManager::GetInstance().
		Play("Hit/Hit.efkefc", effectPos, 0.4f, 0.6f, false);
}
