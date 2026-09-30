#include "Cactas.h"

#include"../../../../System/CollisionManager/CollisionManager.h"

#include"../../../HPBar/EnemyHPBar/EnemyHPBarManager.h"

#include"State/States/CactasNormalState.h"
#include"State/States/CactasDamageState.h"
#include"State//States/CactasDieState.h"

void Cactas::Init()
{
	if (!m_spModel)
	{
		InitCharacterModel("Asset/Models/Enemy/Cactas/Cactas.gltf", "Cactas",
			Math::Vector3(0.0f, 0.5f, 0.0f), 0.4f, "Cactas");

		// アニメーションクラス初期化
		m_animation.Init(m_spModel);

		// パラメータクラス初期化
		m_parameter.Init();

		const auto& param = m_parameter.GetParam();

		m_health.Init(param.m_maxHP);

		m_reachDistance = param.m_reachDistance;
		m_attackCooldownDuration = param.m_attackCooldown;


		m_stateMachine.Start(this);
		m_stateMachine.ChangeState<CactasNormalState>();


		// 近接攻撃パラメータをセット
		m_meleeAttack.m_attackTiming.hitStart = param.m_hitStartFrame;
		m_meleeAttack.m_attackTiming.hitEnd = param.m_hitEndFrame;
		m_meleeAttack.m_sphereRadius = param.m_hitRadius;
		m_meleeAttack.m_forwardOffset = param.m_hitForwardOffset;
		m_meleeAttack.m_knockBackPower = param.m_knockBackPower;

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



void Cactas::OnHit(const AttackInfo attackInfo)
{
	if (ApplyDamage(attackInfo))
	{
		m_stateMachine.ChangeState<CactasDieState>();
	}
	else
	{
		m_stateMachine.ChangeState<CactasDamageState>();
		RePlayAnimation(CactasAnimationType::GetHit);
	}
}

