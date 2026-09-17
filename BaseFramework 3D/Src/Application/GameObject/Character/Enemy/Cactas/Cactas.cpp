#include "Cactas.h"

#include"../../../Effect/EffectManager.h"

#include"../../../../System/CollisionManager/CollisionManager.h"

#include"../../../../System/TimeManager/TimeManager.h"

#include"../../../FlyText/FlyTextManager.h"

#include"../../../HPBar/EnemyHPBar/EnemyHPBarManager.h"

#include"../../Player/Player.h"

#include"State/States/CactasNormalState.h"
#include"State/States/CactasHitShakeState.h"
#include"State//States/CactasDieState.h"

void Cactas::Init()
{
	if (!m_spModel)
	{
		InitEnemyModel("Asset/Models/Enemy/Cactas/Cactas.gltf", "Cactas",
			Math::Vector3(0.0f, 0.5f, 0.0f), 0.4f, "Cactas");

		// アニメーションクラス初期化
		m_animation.Init(m_spModel);

		// パラメータクラス初期化
		m_parameter.Init();

		m_hp = m_parameter.GetParam().m_maxHP;

		m_attackCooldownDuration = 0.5f;

		m_stateMachine.Start(this);
		m_stateMachine.ChangeState<CactasNormalState>();

	}

	EnemyBase::Init();

	CollisionManager::Instance().RegisterObject(CollisionLayer::CharacterBump, shared_from_this());

	SetPos({ 0.0f,0.0f,0.0f });
}

void Cactas::Update()
{

	UpdateGravity();

	m_stateMachine.Update();

	UpdateAttack();
}

void Cactas::PostUpdate()
{
	UpdateAnimation();

	EnemyBase::PostUpdate();

}

void Cactas::SetUpReference()
{

	EnemyBase::SetUpReference();

	// HPBarを生成
	EnemyHPBarManager::Instance().
		CreateHPBar(shared_from_this(),Math::Vector3(-0.7f,1.5f,0.0f));
}

void Cactas::DrawDebug()
{
	DrawBumpDebugSphere(Math::Vector3(0.0f, 0.5f, 0.0f), 0.4f);
	//m_pDebugWire->Draw();
}

void Cactas::DrawParameterInspector()
{
	m_parameter.DrawInspecter();
}

void Cactas::PlayAnimation(CactasAnimationType type)
{
	m_animation.Play(type);
}

void Cactas::RePlayAnimation(CactasAnimationType type)
{
	m_animation.RePlay(type);
}

void Cactas::StartAttack()
{
	m_hitTarget = false;
	SetAttackTiming();
}

void Cactas::EndAttack()
{
	m_attackFlg = false;
	m_attackCooldown = m_attackCooldownDuration;
}

void Cactas::StartHitShake()
{
	float damageRate = std::clamp(m_attackInfo.damage / 50.0f, 0.0f, 1.0f);

	m_hitShakeDuration = std::lerp(0.06f, 0.12f, damageRate);

	m_hitShakePower = std::lerp(0.1f, 0.3f, damageRate);

	m_hitShakeTime = m_hitShakeDuration;

	float hitStopDuration = std::lerp(0.04f, 0.09f, damageRate);

	TimeManager::Instance().StartHitStop(hitStopDuration);


	SetIsHitShake(true);
}

void Cactas::EndHitShake()
{

	AddKnockBack(m_attackInfo.knockBackDir, m_attackInfo.knockBackPower);
}

void Cactas::UpdateLaunch()
{
	if (IsGrounded())
	{
		m_launchFlg = false;
	}

	Math::Vector3 move = m_launchVec * 60.0f * m_deltaTime;
	AddPendingMove(move);
}

void Cactas::UpdateAnimation()
{

	m_animation.Update(m_deltaTime);
}

void Cactas::PlayWalkAnimation()
{
	PlayAnimation(CactasAnimationType::Walk);
}

void Cactas::PlayIdleAnimation()
{
	PlayAnimation(CactasAnimationType::Idle);
}

void Cactas::SetAttackTiming()
{
	m_attackTiming.hitStart=16.0f;
	m_attackTiming.hitEnd=20.0f;

	// フレームを0に
	m_animFrame = 0.0f;
}

void Cactas::UpdateAttackCollision()
{
	UpdateMeleeAttackCollision(0.08f);
}

void Cactas::OnHit(const AttackInfo attackInfo)
{

	m_hp -= attackInfo.damage;

	if (m_hp <= 0)
	{
		m_hp = 0;
		m_outroFlg = true;
		m_stateMachine.ChangeState<CactasDieState>();
	}
	else
	{
		m_attackInfo = attackInfo;
		m_stateMachine.ChangeState<CactasHitShakeState>();
	}

	//EffectManager::Instance().CreateEffect("HitEffect", shared_from_this(),Math::Vector3(0,0.8,-1.0f));

	KdEffekseerManager::GetInstance().
		Play("Hit/Hit.efkefc", GetPos()+Math::Vector3(0.0f,0.5f,0.0f), 0.4f, 1.0f, false);

	FlyTextManager::Instance().CreateDamateText(attackInfo.damage, GetPos());

}
