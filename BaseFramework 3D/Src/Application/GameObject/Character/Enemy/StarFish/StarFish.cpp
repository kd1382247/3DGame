#include "StarFish.h"

#include"../../../../System/CollisionManager/CollisionManager.h"

#include"../../../FlyText/FlyTextManager.h"

#include"../../../HPBar/EnemyHPBar/EnemyHPBarManager.h"

#include"../../Player/Player.h"

#include"State/States/StarFishNormalState.h"
#include"State/States/StarFishDamageState.h"
#include"State/States/StarFishDieState.h"

void StarFish::Init()
{
	if (!m_spModel)
	{
		InitEnemyModel("Asset/Models/Enemy/StarFish/StarFish.gltf", "StarFish",
			Math::Vector3(0.0f, 0.5f, 0.0f), 0.4f, "StarFish");

		// アニメーションクラス初期化
		m_animation.Init(m_spModel);

		// パラメータークラス初期化
		m_parameter.Init();

		m_hp = m_parameter.GetParam().m_maxHP;

		m_attackCooldownDuration = 1.0f;

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
	m_attackTiming.hitStart = 16.0f;
	m_attackTiming.hitEnd = 20.0f;

	// フレームを0に
	m_animFrame = 0.0f;
}

void StarFish::UpdateAttackCollision()
{
	UpdateMeleeAttackCollision(0.08f, m_parameter.GetParam().m_attackPow);
}
