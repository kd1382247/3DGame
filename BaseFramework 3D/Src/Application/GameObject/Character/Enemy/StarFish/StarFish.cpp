#include "StarFish.h"

#include"../../../../System/CollisionManager/CollisionManager.h"

#include"../../../HPBar/EnemyHPBar/EnemyHPBarManager.h"

#include"../../../EnergyBullet/EnergyBulletManager.h"

#include"../../Player/Player.h"

#include"State/States/StarFishNormalState.h"
#include"State/States/StarFishDamageState.h"
#include"State/States/StarFishDieState.h"

void StarFish::Init()
{
	if (!m_spModel)
	{
		InitCharacterModel("Asset/Models/Enemy/StarFish/StarFish.gltf", "StarFish",
			Math::Vector3(0.0f, 0.5f, 0.0f), 0.4f, "StarFish");

		// アニメーションクラス初期化
		m_animation.Init(m_spModel);

		// パラメータークラス初期化
		m_parameter.Init();

		const auto& param = m_parameter.GetParam();

		m_health.Init(param.m_maxHP);

		m_attackCooldownDuration = param.m_attackCooldown;

		// この距離まで近づいたら攻撃(Energy弾を発射)する
		m_reachDistance = param.m_reachDistance;

		m_stateMachine.Start(this);
		m_stateMachine.ChangeState<StarFishNormalState>();
	}

	EnemyBase::Init();

	CollisionManager::Instance().RegisterObject(CollisionLayer::CharacterBump, shared_from_this());

	SetPos({ 0.0f,0.0f,0.0f });
}

void StarFish::Update()
{

	UpdateGravity();

	m_stateMachine.Update();

	UpdateAttack();
}

void StarFish::PostUpdate()
{
	EnemyBase::PostUpdate();

	UpdateAnimation();

	DrawBumpDebugSphere(Math::Vector3(0.0f, 0.5f, 0.0f), 0.4f);

}

void StarFish::DrawParameterInspector()
{
	m_parameter.DrawInspecter();
}

void StarFish::SetUpReference()
{
	EnemyBase::SetUpReference();

	// HPBarを生成
	EnemyHPBarManager::Instance().
		CreateHPBar(shared_from_this(), Math::Vector3(-0.7f, 1.5f, 0.0f));
}

void StarFish::OnHit(const AttackInfo attackInfo)
{
	if (ApplyDamage(attackInfo))
	{
		m_stateMachine.ChangeState<StarFishDieState>();
	}
	else
	{
		m_stateMachine.ChangeState<StarFishDamageState>();
		RePlayAnimation(StarFishAnimationType::GetHit);
	}
}


void StarFish::PlayAnimation(StarFishAnimationType type)
{
	m_animation.Play(type);
}

void StarFish::RePlayAnimation(StarFishAnimationType type)
{
	m_animation.RePlay(type);
}

void StarFish::StartAttack()
{
	m_hitTarget = false;
	m_hasFiredBullet = false;
	SetAttackTiming();
}

void StarFish::UpdateAnimation()
{
	m_animation.Update(m_deltaTime);
}

void StarFish::PlayWalkAnimation()
{
	PlayAnimation(StarFishAnimationType::Walk);
}

void StarFish::PlayIdleAnimation()
{
	PlayAnimation(StarFishAnimationType::Idle);
}

void StarFish::SetAttackTiming()
{
	// 攻撃アニメーション開始からのフレーム数
	m_animFrame = 0.0f;

	// Energy弾を発射するフレームは、Parameterの BulletFireFrame で決める
}

void StarFish::FireEnergyBullet()
{
	auto spPlayer = m_wpPlayer.lock();

	if (!spPlayer)
	{
		return;
	}

	// 正面方向(水平)を作る
	Math::Vector3 attackDir = m_mWorld.Backward();
	attackDir.y = 0;

	if (attackDir.LengthSquared() > 0.000001f)
	{
		attackDir.Normalize();
	}

	const auto& param = m_parameter.GetParam();

	// 発射位置(口元の高さ・少し前方)
	Math::Vector3 spawnPos = GetPos() + Math::Vector3(0.0f, param.m_bulletSpawnHeight, 0.0f) + attackDir * param.m_bulletSpawnForward;

	// プレイヤーへ向かう方向(発射時に一度だけ決定)
	Math::Vector3 dir = (spPlayer->GetPos() + Math::Vector3(0.0f, param.m_bulletAimHeight, 0.0f)) - spawnPos;

	if (dir.LengthSquared() > 0.000001f)
	{
		dir.Normalize();
	}

	EnergyBulletManager::Instance().CreateEnergyBullet(
		spawnPos, dir, param.m_bulletSpeed, param.m_bulletRadius,
		param.m_attackPow, param.m_bulletKnockBack, param.m_bulletLifeTime);
}

void StarFish::UpdateBulletFireTiming()
{
	m_animFrame += 60.0f * m_deltaTime;

	if (m_animFrame < m_parameter.GetParam().m_bulletFireFrame)
	{
		return;
	}

	// アニメーションのタイミングで発射
	FireEnergyBullet();

	m_hasFiredBullet = true;

}
