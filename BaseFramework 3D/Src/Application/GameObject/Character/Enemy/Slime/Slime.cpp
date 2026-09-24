#include "Slime.h"

#include"../../../../Scene/SceneManager.h"
#include"../../../../System/CollisionManager/CollisionManager.h"

#include"../../../HPBar/EnemyHPBar/EnemyHPBarManager.h"
#include"../../../FlyText/FlyTextManager.h"


#include"../../StateMachine/StateMachine.h"
#include"State/States/SlimeNormalState.h"
#include"State/States/SlimeDamageState.h"
#include"State/States//SlimeDieState.h"


void Slime::Init()
{
	if (!m_spModel)
	{
		InitCharacterModel("Asset/Models/Enemy/Slime/Slime.gltf", "Slime",
			Math::Vector3(0.0f, 0.5f, 0.0f), 0.4f, "Slime");

		// アニメーションクラス初期化
		m_animation.Init(m_spModel);

		m_stateMachine.Start(this);
		m_stateMachine.ChangeState<SlimeNormalState>();

		// パラメータクラス初期化
		m_parameter.Init();

		m_attackCooldownDuration = 0.5f;
		m_hp = m_parameter.GetParam(m_slimeSize).m_maxHP;

	}

	EnemyBase::Init();

	CollisionManager::Instance().RegisterObject(CollisionLayer::CharacterBump, shared_from_this());

	// 初期化時に念のため大きさをセット
	SetScale(Math::Vector3::Zero);

}

void Slime::Update()
{

	UpdateGravity();

	m_stateMachine.Update();

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
}

void Slime::SetScale(const Math::Vector3&)
{
	// スライムの大きさはステージ側のScale値に関わらず、常にm_slimeSizeに応じた固定値にする
	const float scale = m_parameter.GetParam(m_slimeSize).m_scale;

	KdGameObject::SetScale(Math::Vector3(scale));
}

void Slime::DrawParameterInspector()
{
	m_parameter.DrawInspecter();
	
	// パラメータ調整中に大きさをいじった時にScaleをすぐに反映
	SetScale(Math::Vector3::Zero);
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
		slime->SetPrevPos(GetPos());
		slime->SetRotation(GetRotation());
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
	UpdateMeleeAttackCollision(0.05f, m_parameter.GetParam(m_slimeSize).m_attackPower);
}

void Slime::OnHit(const AttackInfo attackInfo)
{
	m_hp -= attackInfo.damage;

	if (m_hp <= 0)
	{
		m_hp = 0;
		m_outroFlg = true;
		m_stateMachine.ChangeState<SlimeDieState>();
	}
	else
	{
		m_stateMachine.ChangeState<SlimeDamageState>();
		RePlayAnimation(SlimeAnimationType::GetHit);
	}

	FlyTextManager::Instance().CreateDamateText(attackInfo.damage, GetPos(), m_flyTextPath);

	PlayHitEffect();

	AddKnockBack(attackInfo.knockBackDir, attackInfo.knockBackPower);
}
