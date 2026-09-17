#include "Slime.h"

#include"../../../../Scene/SceneManager.h"
#include"../../../../System/CollisionManager/CollisionManager.h"

#include"../../../HPBar/EnemyHPBar/EnemyHPBarManager.h"
#include"../../../FlyText/FlyTextManager.h"

#include"../../Player/Player.h"

#include"State/SlimeStateMachine.h"
#include"State/States/SlimeNormalState.h"
#include"State/States/SlimeDamageState.h"
#include"State/States//SlimeDieState.h"


void Slime::Init()
{
	if (!m_spModel)
	{
		InitEnemyModel("Asset/Models/Enemy/Slime/Slime.gltf", "Slime",
			Math::Vector3(0.0f, 0.5f, 0.0f), 0.4f, "Slime");

		// アニメーションクラス初期化
		m_animation.Init(m_spModel);

		m_stateMachine.ChangeState(*this, std::make_unique<SlimeNormalState>());

		// パラメータクラス初期化
		m_parameter.Init();

		switch (GetSlimeSize())
		{
		case SlimeSize::Large:

			SetScale(1.5f);

			break;
		case SlimeSize::Small:

			SetScale(1.0f);

			break;
		}

		m_attackCooldownDuration = 0.5f;
		m_hp = m_parameter.GetParam().m_maxHP;
	}

	EnemyBase::Init();

	CollisionManager::Instance().RegisterObject(CollisionLayer::CharacterBump, shared_from_this());

}

void Slime::Update()
{

	UpdateGravity();

	m_stateMachine.Update(*this);
	
	UpdateAttack();
}

void Slime::PostUpdate()
{
	
	UpdateAnimation();

	EnemyBase::PostUpdate();
}

void Slime::SetUpReference()
{

	EnemyBase::SetUpReference();

	// HPBarを生成
	EnemyHPBarManager::Instance().
		CreateHPBar(shared_from_this(), Math::Vector3(-0.7f, 1.5f, 0.0f));
}

void Slime::DrawDebug()
{
	DrawBumpDebugSphere(Math::Vector3(0.0f, 0.5f, 0.0f), 0.4f);
	//m_pDebugWire->Draw();
}

void Slime::DrawParameterInspector()
{
	m_parameter.DrawInspecter();
}

void Slime::PlayAnimation(SlimeAnimationType type)
{
	m_animation.Play(type);
}

void Slime::RePlayAnimation(SlimeAnimationType type)
{
	m_animation.RePlay(type);
}

void Slime::StartAttack()
{
	m_hitTarget = false;
	SetAttackTiming();
}

void Slime::EndAttack()
{
	m_attackFlg = false;
	m_attackCooldown = m_attackCooldownDuration;
}

void Slime::Split()
{
	if (m_slimeSize != SlimeSize::Large)
	{
		return;
	}

	for(int i=0;i<spawnNum;i++)
	{

		std::shared_ptr<Slime>slime = std::make_shared<Slime>();
		slime->SetSlimeSize(SlimeSize::Small);
		slime->Init();
		slime->SetPos(GetPos());
		slime->SetUpReference();

		slime->Launch(m_launchDir[i]*0.05f, 0.3);

		SceneManager::Instance().AddObject(slime);
	}
}

void Slime::UpdateLaunch()
{

	if (IsGrounded())
	{
		m_launchFlg = false;
	}

	Math::Vector3 move = m_launchVec * 60.0f * m_deltaTime;

	AddPendingMove(move);
}

void Slime::UpdateAnimation()
{
	m_animation.Update(m_deltaTime);
}

void Slime::PlayWalkAnimation()
{
	PlayAnimation(SlimeAnimationType::Walk);
}

void Slime::PlayIdleAnimation()
{
	PlayAnimation(SlimeAnimationType::Idle);
}

void Slime::SetAttackTiming()
{
	m_attackTiming.hitStart = 16.0f;
	m_attackTiming.hitEnd = 20.0f;

	// フレームを0に
	m_animFrame = 0.0f;
}

void Slime::UpdateAttackCollision()
{
	UpdateMeleeAttackCollision(0.05f);
}

void Slime::OnHit(const AttackInfo attackInfo)
{
	m_hp -= attackInfo.damage;

	if (m_hp <= 0)
	{
		m_hp = 0;
		m_outroFlg = true;
		ChangeState<SlimeDieState>();
	}
	else
	{
		ChangeState<SlimeDamageState>();
		RePlayAnimation(SlimeAnimationType::GetHit);
	}

	FlyTextManager::Instance().CreateDamateText(attackInfo.damage, GetPos());

	KdEffekseerManager::GetInstance().
		Play("Hit/Hit.efkefc", GetPos() + Math::Vector3(0.0f, 0.5f, 0.0f), 0.4f, 1.0f, false);

	AddKnockBack(attackInfo.knockBackDir, attackInfo.knockBackPower);
}
