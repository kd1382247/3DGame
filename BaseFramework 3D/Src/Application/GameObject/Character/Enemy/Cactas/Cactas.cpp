#include "Cactas.h"

#include"../../../../System/CollisionManager/CollisionManager.h"
#include"../../../../System/WayPointManager/WayPointManager.h"
#include"../../../../GameObject/WayPoint/WayPoint.h"

#include"../../Player/Player.h"

void Cactas::Init()
{
	if (!m_spModel)
	{
		m_spModel = std::make_shared<KdModelWork>();
		m_spModel->SetModelData("Asset/Models/Enemy/Cactas/Cactas.gltf");

		// アニメーションクラス初期化
		m_animation.Init(m_spModel);

		// パラメータクラス初期化
		m_parameter.Init();
		m_turnSpeed = m_parameter.GetParam().m_turnSpeed;
		m_moveSpeed = m_parameter.GetParam().m_moveSpeed;
		m_attackCooldownDuration = 60*0.5;


		m_pCollider = std::make_unique<KdCollider>();
		m_pCollider->RegisterCollisionShape
		("Cactas", Math::Vector3(0, 0.5, 0), 0.4, KdCollider::TypeBump);

		
		m_pDebugWire = std::make_unique<KdDebugWireFrame>();


		// オブジェクト名セット
		SetObjectName("Cactas");
	}

	EnemyBase::Init();

	CollisionManager::Instance().RegisterObject(CollisionLayer::CharacterBump, shared_from_this());

	SetPos({ 5,0,0 });
}

void Cactas::Update()
{

	UpdateMove();

	UpdateAttack();


	UpdateActionState();


	
}

void Cactas::PostUpdate()
{

	UpdateAnimation();

	m_pDebugWire->AddDebugSphere(GetPos() + Math::Vector3(0, 0.5, 0), 0.4, kRedColor);

	EnemyBase::PostUpdate();

}

void Cactas::DrawInspector()
{
	EnemyBase::DrawInspector();

	m_parameter.DrawInspecter();
}

void Cactas::UpdateMove()
{
	Math::Vector3 nowPos = GetPos();
	m_gravity += 0.02;
	nowPos.y -= m_gravity;
	SetPos(nowPos);

	// 攻撃中は移動をしない
	if (m_actionState == CactasActionState::Attack)
	{
		return;
	}

	if (CanDirectChase())
	{
		m_moveState = CactasMoveState::Walk;
	}
	else
	{
		m_moveState = CactasMoveState::Idle;
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

	UpdateFacingDirection();
}

void Cactas::UpdateAttack()
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
	if(m_attackFlg)
	{
		if (m_attackCooldown != 0)
		{
			m_attackFlg = false;
		}
	}
}

void Cactas::UpdateAnimation()
{
	CactasAnimationType nextAnimation = CactasAnimationType::Idle;

	if (m_actionState == CactasActionState::Damage)
	{
		nextAnimation = CactasAnimationType::GetHit;
	}
	else if (m_actionState == CactasActionState::Attack)
	{
		nextAnimation = CactasAnimationType::Attack;
	}
	else if (m_moveState == CactasMoveState::Walk)
	{
		nextAnimation = CactasAnimationType::Walk;
	}
	else
	{
		nextAnimation = CactasAnimationType::Idle;
	}

	m_animation.Play(nextAnimation);
	m_animation.Update();

}

void Cactas::ChangeActionState(CactasActionState nextState)
{
	if (m_actionState == nextState)
	{
		return;
	}

	ExitState(m_actionState);
	m_actionState = nextState;
	EnterState(m_actionState);
	
}

void Cactas::ExitState(CactasActionState _state)
{
	switch (_state)
	{
	case CactasActionState::Normal:

		break;
	case CactasActionState::Attack:
		m_attackFlg = false;
		m_attackCooldown = m_attackCooldownDuration;
		break;
	case CactasActionState::Damage:


		break;
	case CactasActionState::Death:


		break;
	}
}

void Cactas::EnterState(CactasActionState _state)
{
	switch (_state)
	{
	case CactasActionState::Normal:

		break;
	case CactasActionState::Attack:

		break;
	case CactasActionState::Damage:


		break;
	case CactasActionState::Death:


		break;
	}
}

void Cactas::UpdateActionState()
{
	if (m_actionState == CactasActionState::Death)
	{

		return;
	}

	if (m_actionState == CactasActionState::Damage)
	{
		if (m_animation.IsFinished())
		{
			ChangeActionState(CactasActionState::Normal);
		}
		return;
	}

	if (m_actionState == CactasActionState::Attack)
	{
		if (m_animation.IsFinished())
		{
			ChangeActionState(CactasActionState::Normal);
		}
		return;
	}

	if (m_attackFlg)
	{
		ChangeActionState(CactasActionState::Attack);
		return;
	}

}
