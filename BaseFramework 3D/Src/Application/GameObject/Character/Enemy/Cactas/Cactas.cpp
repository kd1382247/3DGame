#include "Cactas.h"

#include"../../../../System/CollisionManager/CollisionManager.h"
#include"../../../../System/WayPointManager/WayPointManager.h"
#include"../../../../GameObject/WayPoint/WayPoint.h"

#include"../../../HPBar/EnemyHPBar/EnemyHPBarManager.h"

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

		m_hp = m_parameter.GetParam().m_maxHP;

		m_attackCooldownDuration = 60*0.5f;

		m_pCollider = std::make_unique<KdCollider>();
		m_pCollider->RegisterCollisionShape
		("Cactas", Math::Vector3(0.0f, 0.5f, 0.0f), 0.4f, KdCollider::TypeBump);

		
		m_pDebugWire = std::make_unique<KdDebugWireFrame>();

		// オブジェクト名セット
		SetObjectName("Cactas");

	}

	EnemyBase::Init();

	CollisionManager::Instance().RegisterObject(CollisionLayer::CharacterBump, shared_from_this());

	SetPos({ 0.0f,0.0f,0.0f });
}

void Cactas::Update()
{

	UpdateGravity();

	if (IsInOutro())
	{
		ChangeActionState(CactasActionState::Death);
		return;
	}

	if (m_launchFlg)
	{
		UpdateLaunch();
		return;
	}

	UpdateMove();

	UpdateAttack();
	
	UpdateAttackCollision();
}

void Cactas::PostUpdate()
{
	UpdateActionState();
	UpdateAnimation();

	EnemyBase::PostUpdate();

}

void Cactas::DrawInspector()
{
	EnemyBase::DrawInspector();

	m_parameter.DrawInspecter();
}

void Cactas::SetUpReference()
{

	EnemyBase::SetUpReference();

	// HPBarを生成
	EnemyHPBarManager::Instance().CreateHPBar(
		std::dynamic_pointer_cast<EnemyBase>(shared_from_this()));
}

void Cactas::DrawDebug()
{
	m_pDebugWire->AddDebugSphere(GetPos() + Math::Vector3(0.0f, 0.5f, 0.0f), 0.4f, kRedColor);
	m_pDebugWire->Draw();
}

void Cactas::UpdateLaunch()
{
	if (IsGrounded())
	{
		m_launchFlg = false;
	}

	Math::Vector3 pos = GetPos();

	pos += m_launchVec;

	SetPos(pos);
}

void Cactas::OutroUpdate()
{
	m_outroFlg = true;
}

void Cactas::UpdateMove()
{

	// 攻撃中は移動をしない
	if (m_actionState == CactasActionState::Attack)
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

	if (m_actionState == CactasActionState::Death)
	{
		nextAnimation = CactasAnimationType::Die;
	}
	else if (m_actionState == CactasActionState::Damage)
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

		m_hitTarget = false;

		SetAttackTiming();

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
		if (m_animation.IsFinished())
		{
			Destroy();
		}
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

void Cactas::SetAttackTiming()
{
	m_attackTiming.hitStart=16.0f;
	m_attackTiming.hitEnd=20.0f;

	// フレームを0に
	m_animFrame = 0.0f;
}

void Cactas::UpdateAttackCollision()
{
	auto spPlayer = m_wpPlayer.lock();
	if (!spPlayer)
	{
		return;
	}

	if (m_actionState != CactasActionState::Attack)
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
		attackInfo.knockBackPower = 0.08f;
		attackInfo.damage = 10;

		spPlayer->OnHit(attackInfo);

		m_hitTarget = true;
	}

	m_pDebugWire->AddDebugSphere(sphere.Center, sphere.Radius, kGreenColor);

}
