#include "Mushroom.h"

#include"../../../../System/CollisionManager/CollisionManager.h"
#include"../../../HPBar/EnemyHPBar/EnemyHPBarManager.h"

#include"State/States/MushroomNormalState.h"
#include"State/States/MushroomAttackState.h"
#include"State/States/MushroomDamageState.h"
#include"State/States/MushroomDieState.h"

void Mushroom::Init()
{
	if (!m_spModel)
	{
		// パラメータクラス初期化(Angryになる確率を使うため、タイプの抽選より先に行う)
		m_parameter.Init();
		ApplyParameter();

		// 一定確率でAngryタイプを抽選する
		LotteryMushroomType();

		const std::string modelPath = (m_mushroomType == MushroomType::Angry) ?
			"Asset/Models/Enemy/Mushroom/MushroomAngry/MushroomAngry.gltf" :
			"Asset/Models/Enemy/Mushroom/MushroomSmile/MushroomSmile.gltf";


		InitCharacterModel(modelPath, "Mushroom", "Mushroom");

		// アニメーションクラス初期化
		m_animation.Init(m_spModel);

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


	if (m_stateMachine.IsState<MushroomNormalState>())
	{
		UpdateAttack();
	}
}

void Mushroom::PostUpdate()
{

	UpdateAnimation();

	EnemyBase::PostUpdate();
}

void Mushroom::DrawParameterInspector()
{
	m_parameter.DrawInspecter();

	// 当たり判定球の変更をすぐに反映
	const auto& bumpParam = m_parameter.GetParam(m_mushroomType);
	SetBumpSphere(bumpParam.m_bumpSphereOffsetY, bumpParam.m_bumpSphereRadius);
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
	if (ApplyDamage(attackInfo))
	{
		m_stateMachine.ChangeState<MushroomDieState>();
	}
	else
	{
		m_stateMachine.ChangeState<MushroomDamageState>();
		RePlayAnimation(MushroomAnimationType::GetHit);
	}
}


void Mushroom::PlayAnimation(MushroomAnimationType type)
{
	m_animation.Play(type);
}

void Mushroom::RePlayAnimation(MushroomAnimationType type)
{
	m_animation.RePlay(type);
}


void Mushroom::ApplyParameter()
{

	const auto& param = m_parameter.GetParam(m_mushroomType);

	// スフィアの高さと半径
	m_bumpSphereOffsetY = param.m_bumpSphereOffsetY;
	m_bumpSphereRadius = param.m_bumpSphereRadius;

	m_health.Init(param.m_maxHP);

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


void Mushroom::LotteryMushroomType()
{
	// Angryタイプになる確率(%)。Parameterで調整できる
	const float angryRate = m_parameter.GetAngryRate();

	m_mushroomType = (KdRandom::GetFloat(0.0f, 100.0f) < angryRate) ?
		MushroomType::Angry : MushroomType::Smile;
}
