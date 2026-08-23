#include "Mushroom.h"

#include"../../../../System/CollisionManager/CollisionManager.h"

void Mushroom::Init()
{
	if (!m_spModel)
	{
		m_spModel = std::make_shared<KdModelWork>();
		m_spModel->SetModelData("Asset/Models/Enemy/Mushroom/MushroomSmile/MushroomSmile.gltf");

		// アニメーションクラス初期化
		m_animation.Init(m_spModel);

		// パラメータクラス初期化
		m_parameter.Init();
		m_turnSpeed = m_parameter.GetParam().m_turnSpeed;
		m_moveSpeed = m_parameter.GetParam().m_moveSpeed;
		m_attackCooldownDuration = 60 * 1;


		m_pCollider = std::make_unique<KdCollider>();
		m_pCollider->RegisterCollisionShape
		("Mushroom", Math::Vector3(0, 0.5, 0), 0.4, KdCollider::TypeBump);


		m_pDebugWire = std::make_unique<KdDebugWireFrame>();

		// オブジェクト名セット
		SetObjectName("Mushroom");
	}

	EnemyBase::Init();

	CollisionManager::Instance().RegisterObject(CollisionLayer::CharacterBump, shared_from_this());


	SetPos({ -5,0,-5});
}

void Mushroom::Update()
{
	UpdateMove();
	UpdateAttack();

	
	UpdateActionState();

}

void Mushroom::PostUpdate()
{
	

	UpdateAnimation();

	m_pDebugWire->AddDebugSphere(GetPos() + Math::Vector3(0, 0.5, 0), 0.4, kRedColor);

	EnemyBase::PostUpdate();

}

void Mushroom::DrawInspector()
{
	EnemyBase::DrawInspector();

	m_parameter.DrawInspecter();
}

void Mushroom::UpdateMove()
{

	Math::Vector3 nowPos = GetPos();
	m_gravity += 0.02;
	nowPos.y -= m_gravity;
	SetPos(nowPos);

	// 攻撃中は移動をしない
	if (m_actionState == MushroomActionState::Attack)
	{
		return;
	}

	if (CanDirectChase())
	{
		m_moveState = MushroomMoveState::Walk;
	}
	else
	{
		m_moveState = MushroomMoveState::Idle;
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

void Mushroom::UpdateAttack()
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

void Mushroom::UpdateAnimation()
{
	MushroomAnimationType nextAnimation = MushroomAnimationType::Idle;

	if (m_actionState == MushroomActionState::Damage)
	{
		nextAnimation = MushroomAnimationType::GetHit;
	}
	else if (m_actionState == MushroomActionState::Attack)
	{
		nextAnimation = MushroomAnimationType::Attack;
	}
	else if (m_moveState == MushroomMoveState::Walk)
	{
		nextAnimation = MushroomAnimationType::Walk;
	}
	else
	{
		nextAnimation = MushroomAnimationType::Idle;
	}

	m_animation.Play(nextAnimation);
	m_animation.Update();

}

void Mushroom::UpdateActionState()
{
	if (m_actionState == MushroomActionState::Death)
	{

		return;
	}

	if (m_actionState == MushroomActionState::Damage)
	{
		if (m_animation.IsFinished())
		{
			ChangeActionState(MushroomActionState::Normal);
		}
		return;
	}

	if (m_actionState == MushroomActionState::Attack)
	{
		if (m_animation.IsFinished())
		{
			ChangeActionState(MushroomActionState::Normal);
		}
		return;
	}

	if (m_attackFlg)
	{
		ChangeActionState(MushroomActionState::Attack);
		return;
	}
}

void Mushroom::ChangeActionState(MushroomActionState nextState)
{
	if (m_actionState == nextState)
	{
		return;
	}

	ExitState(m_actionState);
	m_actionState = nextState;
	EnterState(m_actionState);
}

void Mushroom::ExitState(MushroomActionState _state)
{

	switch (_state)
	{
	case MushroomActionState::Normal:

		break;
	case MushroomActionState::Attack:
		m_attackFlg = false;
		m_attackCooldown = m_attackCooldownDuration;
		break;
	case MushroomActionState::Damage:

		break;
	case MushroomActionState::Death:


		break;
	}
}

void Mushroom::EnterState(MushroomActionState _state)
{


	switch (_state)
	{
	case MushroomActionState::Normal:

		break;
	case MushroomActionState::Attack:
		break;
	case MushroomActionState::Damage:


		break;
	case MushroomActionState::Death:


		break;
	}
}


