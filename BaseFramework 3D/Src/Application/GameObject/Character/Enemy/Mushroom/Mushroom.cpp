#include "Mushroom.h"

#include"../../../../System/CollisionManager/CollisionManager.h"
#include"../../../HPBar/EnemyHPBar/EnemyHPBarManager.h"

#include"../../Player/Player.h"

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

		m_maxHP = m_parameter.GetParam().m_maxHP;
		m_hp = m_maxHP;

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


	SetPos({ 0.0f,0.0f,0.0f });
}

void Mushroom::Update()
{

	if (IsInOutro())
	{
		ChangeActionState(MushroomActionState::Death);
		return;
	}

	UpdateMove();
	UpdateAttack();

	UpdateAttackCollision();

}

void Mushroom::PostUpdate()
{
	
	UpdateActionState();

	UpdateAnimation();

	m_pDebugWire->AddDebugSphere(GetPos() + Math::Vector3(0, 0.5, 0), 0.4, kRedColor);

	EnemyBase::PostUpdate();

}

void Mushroom::DrawInspector()
{
	EnemyBase::DrawInspector();

	m_parameter.DrawInspecter();
}

void Mushroom::SetUpReference()
{
	EnemyBase::SetUpReference();

	// HPBarを生成
	EnemyHPBarManager::Instance().CreateHPBar(
		std::dynamic_pointer_cast<EnemyBase>(shared_from_this()));
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

	// キャラの向き
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

	if (m_actionState == MushroomActionState::Death)
	{
		nextAnimation = MushroomAnimationType::Die;
	}
	else if (m_actionState == MushroomActionState::Damage)
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
		if (m_animation.IsFinished())
		{
			Destroy();
		}

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

void Mushroom::SetAttackTiming()
{
	m_attackTiming.hitStart = 24.0f;
	m_attackTiming.hitEnd = 28.0f;

	// フレームを0に
	m_animFrame = 0.0f;
}

void Mushroom::UpdateAttackCollision()
{
	auto spPlayer = m_wpPlayer.lock();
	if (!spPlayer)
	{
		return;
	}

	if (m_actionState != MushroomActionState::Attack)
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
	sphere.Radius = 0.6;

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
		attackInfo.knockBackPower = 0.05;
		attackInfo.damage = 10;

		spPlayer->OnHit(attackInfo);

		m_hitTarget = true;
	}


	m_pDebugWire->AddDebugSphere(sphere.Center, sphere.Radius, kGreenColor);
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

		m_hitTarget = false;
		SetAttackTiming();


		break;
	case MushroomActionState::Damage:


		break;
	case MushroomActionState::Death:


		break;
	}
}


