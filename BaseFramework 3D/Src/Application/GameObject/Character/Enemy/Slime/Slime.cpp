#include "Slime.h"

#include"../../../../Scene/SceneManager.h"
#include"../../../../System/CollisionManager/CollisionManager.h"

#include"../../../HPBar/EnemyHPBar/EnemyHPBarManager.h"

#include"../../Player/Player.h"

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


		// パラメータクラス初期化
		m_parameter.Init();

		switch (GetSlimeSize())
		{
		case SlimeSize::Large:

			m_maxHP = m_parameter.GetParam().m_maxHP;
			SetScale(2.0f);

			break;
		case SlimeSize::Small:

			// HPを半分にする
			m_maxHP = m_parameter.GetParam().m_maxHP/2;
			SetScale(1.0f);

			break;
		}

		m_turnSpeed = m_parameter.GetParam().m_turnSpeed;
		m_moveSpeed = m_parameter.GetParam().m_moveSpeed;
		m_attackPower = m_parameter.GetParam().m_attackPower;
		m_attackCooldownDuration = 60 * 0.5;
		m_hp = m_maxHP;
	}

	EnemyBase::Init();

	CollisionManager::Instance().RegisterObject(CollisionLayer::CharacterBump, shared_from_this());

}

void Slime::Update()
{

	UpdateGravity();

	if (IsInOutro())
	{
		ChangeActionState(SlimeActionState::Death);
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

void Slime::PostUpdate()
{
	
	UpdateActionState();
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
	m_pDebugWire->Draw();
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
	
	// 攻撃中は移動をしない
	if (m_actionState == SlimeActionState::Attack)
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
		m_moveState = SlimeMoveState::Walk;
	}
	else
	{
		m_moveState = SlimeMoveState::Idle;
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

void Slime::UpdateActionState()
{
	if (m_actionState == SlimeActionState::Death)
	{
		if (m_animation.IsFinished())
		{
			Split();
			Destroy();
		}

		return;
	}

	if (m_actionState == SlimeActionState::Damage)
	{
		if (m_animation.IsFinished())
		{
			ChangeActionState(SlimeActionState::Normal);
		}
		return;
	}

	if (m_actionState == SlimeActionState::Attack)
	{
		if (m_animation.IsFinished())
		{
			ChangeActionState(SlimeActionState::Normal);
		}
		return;
	}

	if (m_attackFlg)
	{
		ChangeActionState(SlimeActionState::Attack);
		return;
	}
}

void Slime::UpdateAnimation()
{
	SlimeAnimationType nextAnimation = SlimeAnimationType::Idle;

	if (m_actionState == SlimeActionState::Death)
	{
		nextAnimation = SlimeAnimationType::Die;
	}	
	else if (m_actionState == SlimeActionState::Damage)
	{
		nextAnimation = SlimeAnimationType::GetHit;
	}
	else if (m_actionState == SlimeActionState::Attack)
	{
		nextAnimation = SlimeAnimationType::Attack;
	}
	else if (m_moveState == SlimeMoveState::Walk)
	{
		nextAnimation = SlimeAnimationType::Walk;
	}
	else
	{
		nextAnimation = SlimeAnimationType::Idle;
	}

	m_animation.Play(nextAnimation);
	m_animation.Update();

}

void Slime::ChangeActionState(SlimeActionState nextState)
{
	if (m_actionState == nextState)
	{
		return;
	}

	ExitState(m_actionState);
	m_actionState = nextState;
	EnterState(m_actionState);
}

void Slime::ExitState(SlimeActionState _state)
{
	switch (_state)
	{
	case SlimeActionState::Normal:

		break;
	case SlimeActionState::Attack:
		m_attackFlg = false;
		m_attackCooldown = m_attackCooldownDuration;
		break;
	case SlimeActionState::Damage:


		break;
	case SlimeActionState::Death:


		break;
	}
}

void Slime::EnterState(SlimeActionState _state)
{
	switch (_state)
	{
	case SlimeActionState::Normal:

		break;
	case SlimeActionState::Attack:

		m_hitTarget = false;
		SetAttackTiming();

		break;
	case SlimeActionState::Damage:

		break;
	case SlimeActionState::Death:


		break;
	}
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

	if (m_actionState != SlimeActionState::Attack)
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
