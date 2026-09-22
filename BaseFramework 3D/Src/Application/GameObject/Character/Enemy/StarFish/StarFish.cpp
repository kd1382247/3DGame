#include "StarFish.h"

#include"../../../../System/CollisionManager/CollisionManager.h"

#include"../../../FlyText/FlyTextManager.h"

#include"../../../HPBar/EnemyHPBar/EnemyHPBarManager.h"

#include"../../Player/Player.h"

#include"../../../EnergyBullet/EnergyBulletManager.h"

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

		m_hp = m_parameter.GetParam().m_maxHP;

		m_attackCooldownDuration = 1.0f;

		// 5m以内に近づいたら攻撃(Energy弾を発射)する
		m_reachDistance = 5.0f;

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
	m_hp -= attackInfo.damage;

	if (m_hp <= 0)
	{
		m_hp = 0;
		m_outroFlg = true;
		m_stateMachine.ChangeState<StarFishDieState>();
	}
	else
	{
		m_stateMachine.ChangeState<StarFishDamageState>();
		RePlayAnimation(StarFishAnimationType::GetHit);
	}

	FlyTextManager::Instance().CreateDamateText(attackInfo.damage, GetPos());

	KdEffekseerManager::GetInstance().
		Play("Hit/Hit.efkefc", GetPos() + Math::Vector3(0.0f, 0.5f, 0.0f), 0.4f, 1.0f, false);

	AddKnockBack(attackInfo.knockBackDir, attackInfo.knockBackPower);
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

void StarFish::EndAttack()
{
	m_attackFlg = false;
	m_attackCooldown = m_attackCooldownDuration;
}

void StarFish::UpdateLaunch()
{
	if (IsGrounded())
	{
		m_launchFlg = false;
	}

	Math::Vector3 move = m_launchVec * 60.0f * m_deltaTime;

	AddPendingMove(move);
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

	// Energy弾を発射するフレーム(m_bulletFireFrameはヘッダで初期化済み)
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

	// 発射位置(口元の高さ・少し前方)
	Math::Vector3 spawnPos = GetPos() + Math::Vector3(0.0f, 0.8f, 0.0f) + attackDir * 0.3f;

	// プレイヤーへ向かう方向(発射時に一度だけ決定)
	Math::Vector3 dir = (spPlayer->GetPos()+Math::Vector3(0.0f,0.8f,0.0f)) - spawnPos;

	if (dir.LengthSquared() > 0.000001f)
	{
		dir.Normalize();
	}

	EnergyBulletManager::Instance().CreateEnergyBullet(
		spawnPos, dir, /*speed=*/0.08f, /*radius=*/0.05f,
		/*damage=*/m_parameter.GetParam().m_attackPow, /*knockBackPower=*/0.08f, /*lifeTime=*/4.0f);
}

void StarFish::UpdateBulletFireTiming()
{
	m_animFrame += 60.0f * m_deltaTime;

	if (m_animFrame < m_bulletFireFrame)
	{
		return;
	}

	// アニメーションのタイミングで発射
	FireEnergyBullet();

	m_hasFiredBullet = true;

}
