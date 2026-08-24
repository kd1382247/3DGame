#include "TurtleShell.h"

#include"../../../../System/CollisionManager/CollisionManager.h"
#include"../../../Stage/Stage01/Collision/WallCollision/WallCollisionManager.h"
#include"../../../Stage/Stage01/Collision/WallCollision/WallCollision.h"

#include"../../Player/Player.h"


void TurtleShell::Init()
{
	if (!m_spModel)
	{
		m_spModel = std::make_shared<KdModelWork>();
		m_spModel->SetModelData("Asset/Models/Enemy/TurtleShell/TurtleShell.gltf");

		// アニメーションクラス初期化
		m_animation.Init(m_spModel);

		// パラメータクラス初期化
		m_parameter.Init();
		m_turnSpeed = m_parameter.GetParam().m_turnSpeed;
		m_moveSpeed = m_parameter.GetParam().m_moveSpeed;
		m_hp = m_parameter.GetParam().m_maxHP;

		m_attackCooldownDuration = 60 * 1.0f;
		m_dizzyDuration = 60 * 3.0f;
		m_spinAttackDuration = 60 * 5.0f;
		m_hitCooldownDuration = 60 * 0.5f;


		m_pCollider = std::make_unique<KdCollider>();
		m_pCollider->RegisterCollisionShape
		("TurtleShell", Math::Vector3(0, 0.5, 0), 0.4, KdCollider::TypeBump);

		m_pDebugWire = std::make_unique<KdDebugWireFrame>();

		// オブジェクト名セット
		SetObjectName("TurtleShell");
	}

	EnemyBase::Init();

	CollisionManager::Instance().RegisterObject(CollisionLayer::CharacterBump, shared_from_this());

	SetPos({ 0,0,-10 });
}

void TurtleShell::Update()
{

	////// 現在のオブジェクト数をデバッグ
	//KdDebugGUI::Instance().ClearLog();

	//KdDebugGUI::Instance().AddLog("HP%f\n", m_hp);
	//KdDebugGUI::Instance().AddLog("moveDir.x%f\n", GetMoveDir().x);
	//KdDebugGUI::Instance().AddLog("moveDir.y%f\n", GetMoveDir().y);
	//KdDebugGUI::Instance().AddLog("moveDir.z%f\n", GetMoveDir().z);


	UpdateMove();

	UpdateSpinAttackMove();
	UpdateAttack();

	UpdateActionState();

	UpdateAttackCollision();

}

void TurtleShell::PostUpdate()
{
	EnemyBase::PostUpdate();

	UpdateAnimation();

	m_pDebugWire->AddDebugSphere(GetPos() + Math::Vector3(0, 0.5, 0), 0.4, kRedColor);

}

void TurtleShell::DrawInspector()
{
	EnemyBase::DrawInspector();

	m_parameter.DrawInspecter();
}

void TurtleShell::UpdateMove()
{

	Math::Vector3 nowPos = GetPos();
	m_gravity += 0.02;
	nowPos.y -= m_gravity;
	SetPos(nowPos);


	if (m_actionState == TurtleShellActionState::Dizzy||
		m_actionState==TurtleShellActionState::SpinAttackRPT)
	{
		return;
	}

	if (m_knockBack != Math::Vector3::Zero)
	{
		return;
	}

	if (CanDirectChase())
	{
		m_moveState = TurtleShellMoveState::Walk;
	}
	else
	{
		m_moveState = TurtleShellMoveState::Idle;
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

void TurtleShell::UpdateActionState()
{
	if (m_actionState == TurtleShellActionState::Death)
	{

		return;
	}

	if (m_actionState == TurtleShellActionState::Damage)
	{
		if (m_animation.IsFinished())
		{
			ChangeActionState(TurtleShellActionState::Normal);
		}
		return;
	}

	// 回転攻撃
	if (m_actionState == TurtleShellActionState::SpinAttackST)
	{
		if (m_animation.IsFinished())
		{
			ChangeActionState(TurtleShellActionState::SpinAttackRPT);
		}
		return;
	}

	if (m_actionState == TurtleShellActionState::SpinAttackRPT)
	{
		if(SpinAttackRemaining())
		{
			ChangeActionState(TurtleShellActionState::Dizzy);
		}
		return;
	}

	// ふらつきのアニメーション
	if (m_actionState == TurtleShellActionState::Dizzy)
	{
		if (DizyyRemaining())
		{
			ChangeActionState(TurtleShellActionState::Normal);
		}
		return;
	}

	if (m_attackFlg)
	{
		ChangeActionState(TurtleShellActionState::SpinAttackST);
		return;
	}

}

void TurtleShell::UpdateAttackCollision()
{
	
	auto spPlayer = m_wpPlayer.lock();
	if (!spPlayer)
	{
		return;
	}

	if (m_actionState != TurtleShellActionState::SpinAttackRPT)
	{
		return;
	}

	// 攻撃が当たっていたら
	if (m_hitTarget)
	{
		HitCoolDownRemaining();
		return;
	}
	
	DirectX::BoundingSphere sphere;

	sphere.Center = GetPos()+Math::Vector3(0.0f,0.5f,0.0f);
	sphere.Radius = 0.5;

	KdCollider::SphereInfo sphereInfo(KdCollider::TypeBump, sphere);

	if (spPlayer->Intersects(sphereInfo, nullptr))
	{	
		// ノックバックの方向を作成
		Math::Vector3 knockBackDir = spPlayer->GetPos()-GetPos();
		knockBackDir.y = 0;
		if (knockBackDir.LengthSquared() > 0.000001f)
		{
			knockBackDir.Normalize();
		}

		AttackInfo attackInfo;

		attackInfo.knockBackDir = knockBackDir;
		attackInfo.knockBackPower = 0.3;
		attackInfo.damage = 10;

		spPlayer->OnHit(attackInfo);

		m_hitTarget = true;
	}

	m_pDebugWire->AddDebugSphere(sphere.Center, sphere.Radius, kGreenColor);

}


void TurtleShell::UpdateAttack()
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

void TurtleShell::UpdateSpinAttackMove()
{

	if (m_actionState != TurtleShellActionState::SpinAttackRPT)
	{
		return;
	}

	if (m_wpPlayer.lock())
	{
		m_targetPos = m_wpPlayer.lock()->GetPos();
	}

	Math::Vector3 moveDir = GetMoveDir();
	moveDir.Normalize();

	for (auto wall : WallCollisionManager::Instance().GetWallCollisionList())
	{
		if (!wall)
		{
			continue;
		}

		Math::Vector3 push;
		Math::Vector3 normal;

		if (CollisionManager::Instance().SphereVsAABB(
			GetBumpSphere(), wall->GetBox(),push,normal))
		{
			// ボックスとぶつかったら反転
			// 反射ベクトルを求める
			float dot = moveDir.Dot(normal);

			if(dot<0.0f)
			{
				Math::Vector3 R = moveDir - 2 * dot * normal;
				R.Normalize();
				moveDir = R;

				SetMoveDir(moveDir);
			}
		}
	}


	Math::Vector3 pos = GetPos();

	pos += moveDir * (m_moveSpeed+0.07);
	SetPos(pos);
}

bool TurtleShell::SpinAttackRemaining()
{
	
	m_spinAttackRemaining--;

	if (m_spinAttackRemaining <= 0)
	{
		return true;
	}

	return false;
}

bool TurtleShell::DizyyRemaining()
{

	m_dizzyRemaining--;

	if (m_dizzyRemaining <= 0)
	{
		return true;
	}

	return false;
}

void TurtleShell::HitCoolDownRemaining()
{
	m_hitCooldownRemaining--;

	if (m_hitCooldownRemaining<= 0)
	{
		m_hitCooldownRemaining = m_hitCooldownDuration;
		m_hitTarget = false;
	}
}

void TurtleShell::UpdateAnimation()
{
	TurtleShellAnimationType nextAnimation = TurtleShellAnimationType::Idle;

	if (m_actionState == TurtleShellActionState::Damage)
	{
		nextAnimation = TurtleShellAnimationType::GetHit;
	}
	else if (m_actionState == TurtleShellActionState::SpinAttackST)
	{
		nextAnimation = TurtleShellAnimationType::SpinAttackST;
	}
	else if (m_actionState == TurtleShellActionState::SpinAttackRPT)
	{
		nextAnimation = TurtleShellAnimationType::SpinAttackRPT;
	}
	else if (m_actionState == TurtleShellActionState::Dizzy)
	{
		nextAnimation = TurtleShellAnimationType::Dizzy;
	}
	else if (m_moveState == TurtleShellMoveState::Walk)
	{
		nextAnimation = TurtleShellAnimationType::Walk;
	}
	else
	{
		nextAnimation = TurtleShellAnimationType::Idle;
	}

	m_animation.Play(nextAnimation);
	m_animation.Update();

}

void TurtleShell::ChangeActionState(TurtleShellActionState nextState)
{
	if (m_actionState == nextState)
	{
		return;
	}

	ExitState(m_actionState);
	m_actionState = nextState;
	EnterState(m_actionState);
}

void TurtleShell::ExitState(TurtleShellActionState _state)
{
	switch (_state)
	{
	case TurtleShellActionState::Normal:

		break;
	case TurtleShellActionState::SpinAttackST:

		
		break;
	case TurtleShellActionState::SpinAttackRPT:

		m_bumpPushRate = 1.0f;
		
		break;
	case TurtleShellActionState::Dizzy:

		m_attackFlg = false;
		m_attackCooldown = m_attackCooldownDuration;
		
		break;
	case TurtleShellActionState::Damage:


		break;
	case TurtleShellActionState::Death:

		break;
	}
}

void TurtleShell::EnterState(TurtleShellActionState _state)
{

	switch (_state)
	{
	case TurtleShellActionState::Normal:

		break;
	case TurtleShellActionState::SpinAttackST:

		break;
	case TurtleShellActionState::SpinAttackRPT:

		// 回転攻撃の持続時間
		m_spinAttackRemaining = m_spinAttackDuration;

		m_hitCooldownRemaining = m_hitCooldownDuration;

		m_bumpPushRate = 0.0f;

		m_hitTarget = false;

		break;
	case TurtleShellActionState::Dizzy:

		m_dizzyRemaining = m_dizzyDuration;

		break;
	case TurtleShellActionState::Damage:
		break;
	case TurtleShellActionState::Death:

		break;
	}
}
