#include "Mushroom.h"

#include"../../../../System/CollisionManager/CollisionManager.h"
#include"../../../HPBar/EnemyHPBar/EnemyHPBarManager.h"
#include"../../../FlyText/FlyTextManager.h"

#include"../../Player/Player.h"

#include"State/States/MushroomNormalState.h"
#include"State/States/MushroomDamageState.h"
#include"State/States/MushroomDieState.h"

void Mushroom::Init()
{
	if (!m_spModel)
	{
		InitEnemyModel("Asset/Models/Enemy/Mushroom/MushroomSmile/MushroomSmile.gltf", "Mushroom",
			Math::Vector3(0.0f, 0.5f, 0.0f), 0.4f, "Mushroom");

		// アニメーションクラス初期化
		m_animation.Init(m_spModel);

		// パラメータクラス初期化
		m_parameter.Init();

		m_hp = m_parameter.GetParam().m_maxHP;

		m_attackCooldownDuration = 1.0f;

		m_stateMachine.Start(this);
		m_stateMachine.ChangeState<MushroomNormalState>();

	}

	EnemyBase::Init();

	CollisionManager::Instance().RegisterObject(CollisionLayer::CharacterBump, shared_from_this());


	SetPos({ 0.0f,0.0f,0.0f });
}

void Mushroom::Update()
{

	UpdateGravity();

	m_stateMachine.Update();

	UpdateAttack();

}

void Mushroom::PostUpdate()
{

	UpdateAnimation();

	EnemyBase::PostUpdate();
}

void Mushroom::DrawDebug()
{
	DrawBumpDebugSphere(Math::Vector3(0.0f, 0.5f, 0.0f), 0.4f);
	//m_pDebugWire->Draw();
}

void Mushroom::DrawParameterInspector()
{
	m_parameter.DrawInspecter();
}

void Mushroom::SetUpReference()
{
	EnemyBase::SetUpReference();

	// HPBarを生成
	EnemyHPBarManager::Instance().
		CreateHPBar(shared_from_this(), Math::Vector3(-0.7f, 1.5f, 0.0f));
}

void Mushroom::OnHit(const AttackInfo attackInfo)
{
	m_hp -= attackInfo.damage;

	if (m_hp <= 0)
	{
		m_hp = 0;
		m_outroFlg = true;
		m_stateMachine.ChangeState<MushroomDieState>();
	}
	else
	{
		m_stateMachine.ChangeState<MushroomDamageState>();
		RePlayAnimation(MushroomAnimationType::GetHit);
	}

	FlyTextManager::Instance().CreateDamateText(attackInfo.damage, GetPos());

	KdEffekseerManager::GetInstance().
		Play("Hit/Hit.efkefc", GetPos() + Math::Vector3(0.0f, 0.5f, 0.0f), 0.4f, 1.0f, false);

	AddKnockBack(attackInfo.knockBackDir, attackInfo.knockBackPower);
}

void Mushroom::PlayAnimation(MushroomAnimationType type)
{
	m_animation.Play(type);
}

void Mushroom::RePlayAnimation(MushroomAnimationType type)
{
	m_animation.RePlay(type);
}

void Mushroom::StartAttack()
{
	m_hitTarget = false;
	SetAttackTiming();
}

void Mushroom::EndAttack()
{
	m_attackFlg = false;
	m_attackCooldown = m_attackCooldownDuration;
}

void Mushroom::UpdateLaunch()
{
	if (IsGrounded())
	{
		m_launchFlg = false;
	}

	Math::Vector3 move = m_launchVec * 60.0f * m_deltaTime;

	AddPendingMove(move);
}

void Mushroom::UpdateAnimation()
{
	m_animation.Update(m_deltaTime);
}

void Mushroom::PlayWalkAnimation()
{
	PlayAnimation(MushroomAnimationType::Walk);
}

void Mushroom::PlayIdleAnimation()
{
	PlayAnimation(MushroomAnimationType::Idle);
}

void Mushroom::SetAttackTiming()
{
	m_attackTiming.hitStart = 24.0f;
	m_attackTiming.hitEnd = 28.0f;

	// フレームを0に
	m_animFrame = 0.0f;
}

void Mushroom::UpdateAttackCollision()
{
	UpdateMeleeAttackCollision(0.05f);
}
