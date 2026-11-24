#include "Cactas.h"

#include"../../../../System/CollisionManager/CollisionManager.h"

#include"../../../HPBar/EnemyHPBar/EnemyHPBarManager.h"

#include"State/States/CactasNormalState.h"
#include"State/States/CactasAttackState.h"
#include"State/States/CactasDamageState.h"
#include"State//States/CactasDieState.h"

void Cactas::Init()
{
	if (!m_spModel)
	{
		// パラメータ初期化(当たり判定の大きさに使うので、モデル生成より先に読み込む)
		m_parameter.Init();

		ApplyParameter();

		InitCharacterModel("Asset/Models/Enemy/Cactas/Cactas.gltf", "Cactas", "Cactas");

		// アニメーションクラス初期化
		m_animation.Init(m_spModel);

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

	if(m_stateMachine.IsState<CactasNormalState>())
	{
		UpdateAttack();
	}
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

void Cactas::DrawParameterInspector()
{
	m_parameter.DrawInspecter();

	// 当たり判定球の変更をすぐに反映
	const auto& bumpParam = m_parameter.GetParam();
	SetBumpSphere(bumpParam.m_bumpSphereOffsetY, bumpParam.m_bumpSphereRadius);
}

void Cactas::PlayAnimation(CactasAnimationType type)
{
	m_animation.Play(type);
}

void Cactas::RePlayAnimation(CactasAnimationType type)
{
	m_animation.RePlay(type);
}

void Cactas::ApplyParameter()
{
	const auto& param = m_parameter.GetParam();

	// スフィアの高さと半径
	m_bumpSphereOffsetY = param.m_bumpSphereOffsetY;
	m_bumpSphereRadius  = param.m_bumpSphereRadius;

	m_health.Init(param.m_maxHP);

	// 当たり判定(押し戻し)球の大きさをパラメータから反映
	SetBumpSphere(param.m_bumpSphereOffsetY, param.m_bumpSphereRadius);

	// 重力加速度をパラメータから反映
	SetGravityAcceleration(param.m_gravityAcceleration);

	m_reachDistance = param.m_reachDistance;
	m_attackCooldownDuration = param.m_attackCooldown;

	// 近接攻撃パラメータをセット
	m_meleeAttack.m_attackTiming.hitStart = param.m_hitStartFrame;
	m_meleeAttack.m_attackTiming.hitEnd = param.m_hitEndFrame;
	m_meleeAttack.m_sphereRadius = param.m_hitRadius;
	m_meleeAttack.m_forwardOffset = param.m_hitForwardOffset;
	m_meleeAttack.m_knockBackPower = param.m_knockBackPower;

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

