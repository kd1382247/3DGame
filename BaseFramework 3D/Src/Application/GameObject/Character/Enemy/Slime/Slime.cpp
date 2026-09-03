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
		m_spModel = std::make_shared<KdModelWork>();
		m_spModel->SetModelData("Asset/Models/Enemy/Slime/Slime.gltf");

		// アニメーションクラス初期化
		m_animation.Init(m_spModel);

		
		m_pCollider = std::make_unique<KdCollider>();
		m_pCollider->RegisterCollisionShape
		("Slime", Math::Vector3(0.0f, 0.5f, 0.0f), 0.4f, KdCollider::TypeBump);

		m_pDebugWire = std::make_unique<KdDebugWireFrame>();

		// オブジェクト名セット
		SetObjectName("Slime");


		m_stateMachine.ChangeState(*this, std::make_unique<SlimeNormalState>());

		// パラメータクラス初期化
		m_parameter.Init();

		switch (GetSlimeSize())
		{
		case SlimeSize::Large:

			SetScale(2.0f);

			break;
		case SlimeSize::Small:

			SetScale(1.0f);

			break;
		}

		m_attackCooldownDuration = 60 * 0.5;
		m_hp = m_parameter.GetParam().m_maxHP;
	}

	EnemyBase::Init();

	CollisionManager::Instance().RegisterObject(CollisionLayer::CharacterBump, shared_from_this());

}

void Slime::Update()
{

	UpdateGravity();

	//m_stateMachine.Update(*this);
	
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
	if (EnemyHPBarManager::Instance().CreateHPBar(
		std::dynamic_pointer_cast<EnemyBase>(shared_from_this())))
	{
		return;
	}
}

void Slime::DrawInspector()
{
	EnemyBase::DrawInspector();

	m_parameter.DrawInspecter();
}

void Slime::DrawDebug()
{

	m_pDebugWire->AddDebugSphere(GetPos() + Math::Vector3(0.0f, 0.5f, 0.0f), 0.4f, kRedColor);
	//m_pDebugWire->Draw();
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

	Math::Vector3 pos = GetPos();

	pos += m_launchVec;

	SetPos(pos);
}

void Slime::UpdateMove()
{
	if (m_knockBack != Math::Vector3::Zero)
	{
		// キャラの向き
		auto spPlayer = m_wpPlayer.lock();
		if (!spPlayer)
		{
			return;
		}

		Math::Vector3 toDir = spPlayer->GetPos() - GetPos();
		SetMoveDir(toDir);
		UpdateFacingDirection();

		return;
	}

	if (CanDirectChase())
	{
		PlayAnimation(SlimeAnimationType::Walk);
	}
	else
	{
		PlayAnimation(SlimeAnimationType::Idle);
	}

	ChangeMoveState(m_nextMoveState);

	switch (m_currentMoveState)
	{
	case EnemyBase::MoveState::DirectChase:
		UpdateDirectChase();
		break;
	case EnemyBase::MoveState::FollowPath:
		UpdateFollowPath();
		break;
	}

	// キャラの向き
	UpdateFacingDirection();

}

void Slime::UpdateAttack()
{
	// ターゲットに到達したら攻撃する
	if (m_hasReachedTarget)
	{
		m_attackFlg = true;
	}

	m_attackCooldown--;
	if (m_attackCooldown <= 0)
	{
		m_attackCooldown = 0;
	}

	// クールタイムがある場合は攻撃しない
	if (m_attackFlg)
	{
		if (m_attackCooldown != 0)
		{
			m_attackFlg = false;
		}
	}
}

void Slime::UpdateAnimation()
{
	m_animation.Update();
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
	auto spPlayer = m_wpPlayer.lock();
	if (!spPlayer)
	{
		return;
	}

	// 攻撃が当たっていたら
	if (m_hitTarget)
	{
		return;
	}

	m_animFrame++;

	if (m_animFrame <= m_attackTiming.hitStart || m_animFrame >= m_attackTiming.hitEnd)
	{
		return;
	}

	// 攻撃する位置
	Math::Vector3 attackPos = GetPos() + Math::Vector3(0.0f, 0.5f, 0.0f);

	// 攻撃する方向
	Math::Vector3 attackDir = m_mWorld.Backward();
	attackDir.y = 0;

	if (attackDir.LengthSquared() <= 0.000001f)
	{
		return;
	}

	// プレイヤーの少し前に出す
	attackPos += attackDir * 0.8f;

	DirectX::BoundingSphere sphere;

	sphere.Center = attackPos;
	sphere.Radius = 0.6f;

	KdCollider::SphereInfo sphereInfo(KdCollider::TypeBump, sphere);

	if (spPlayer->Intersects(sphereInfo, nullptr))
	{
		// ノックバックの方向を作る
		Math::Vector3 knockBackDir = spPlayer->GetPos() - GetPos();
		knockBackDir.y = 0;
		if (knockBackDir.LengthSquared() > 0.000001f)
		{
			knockBackDir.Normalize();
		}

		AttackInfo attackInfo;

		attackInfo.knockBackDir = knockBackDir;
		attackInfo.knockBackPower = 0.05f;
		attackInfo.damage = 10;

		spPlayer->OnHit(attackInfo);

		m_hitTarget = true;
	}


	m_pDebugWire->AddDebugSphere(sphere.Center, sphere.Radius, kGreenColor);
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

	AddKnockBack(attackInfo.knockBackDir, attackInfo.knockBackPower);
}
