#include "TurtleShell.h"

#include"../../../../System/CollisionManager/CollisionManager.h"
#include"../../../../System/CollisionManager/CollisionMath/CollisionMath.h"
#include"../../../Stage/Stage01/Collision/WallCollision/WallCollisionManager.h"
#include"../../../Stage/Stage01/Collision/WallCollision/WallCollision.h"

#include"../../../HPBar/EnemyHPBar/EnemyHPBarManager.h"
#include"../../../FlyText/FlyTextManager.h"

#include"../../Player/Player.h"

#include"State/States/TurtleShellNormalState.h"
#include"State/States/TurtleShellDamageState.h"
#include"State/States/TurtleShellDieState.h"

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
		
		m_hp = m_parameter.GetParam().m_maxHP;

		m_attackCooldownDuration = 1.0f;
		m_dizzyDuration = 3.0f;
		m_spinAttackDuration = 5.0f;
		m_hitCooldownDuration =  0.5f;


		m_pCollider = std::make_unique<KdCollider>();
		m_pCollider->RegisterCollisionShape
		("TurtleShell", Math::Vector3(0.0f, 0.5f, 0.0f), 0.4f, KdCollider::TypeBump);

		m_pDebugWire = std::make_unique<KdDebugWireFrame>();

		// オブジェクト名セット
		SetObjectName("TurtleShell");


		m_stateMachine.ChangeState (*this, std::make_unique<TurtleShellNormalState>());
	}

	EnemyBase::Init();

	CollisionManager::Instance().RegisterObject(CollisionLayer::CharacterBump, shared_from_this());

	SetPos({ 0.0f,0.0f,0.0f });
}

void TurtleShell::Update()
{
	UpdateGravity();

	m_stateMachine.Update(*this);

	UpdateAttack();

}

void TurtleShell::PostUpdate()
{
	
	UpdateAnimation();

	EnemyBase::PostUpdate();
}

void TurtleShell::DrawInspector()
{
	EnemyBase::DrawInspector();

	m_parameter.DrawInspecter();
}

void TurtleShell::DrawDebug()
{
	m_pDebugWire->AddDebugSphere(GetPos() + Math::Vector3(0.0f, 0.5f, 0.0f), 0.4f, kRedColor);
	//m_pDebugWire->Draw();
}

void TurtleShell::SetUpReference()
{
	EnemyBase::SetUpReference();


	// HPBarを生成
	EnemyHPBarManager::Instance().CreateHPBar(
		std::dynamic_pointer_cast<EnemyBase>(shared_from_this()));
}

void TurtleShell::PlayAnimation(TurtleShellAnimationType type)
{
	m_animation.Play(type);
}

void TurtleShell::RePlayAnimation(TurtleShellAnimationType type)
{
	m_animation.RePlay(type);
}

void TurtleShell::StartSpinAttack()
{
	// 回転攻撃の持続時間
	m_spinAttackRemaining = m_spinAttackDuration;

	m_hitCooldownRemaining = m_hitCooldownDuration;

	m_bumpPushRate = 0.0f;

	m_hitTarget = false;
}

void TurtleShell::EndSpinAttack()
{
	m_bumpPushRate = 1.0f;
}

void TurtleShell::StartDizzy()
{
	m_dizzyRemaining = m_dizzyDuration;
}

void TurtleShell::EndDizzy()
{
	m_attackFlg = false;
	m_attackCooldown = m_attackCooldownDuration;
}

void TurtleShell::UpdateLaunch()
{
	if (IsGrounded())
	{
		m_launchFlg = false;
	}

	Math::Vector3 move = m_launchVec * 60.0f * m_deltaTime;
	AddPendingMove(move);
}

void TurtleShell::UpdateMove()
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
		PlayAnimation(TurtleShellAnimationType::Walk);
	}
	else
	{
		PlayAnimation(TurtleShellAnimationType::Idle);
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

void TurtleShell::UpdateAttackCollision()
{
	
	auto spPlayer = m_wpPlayer.lock();
	if (!spPlayer)
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
		attackInfo.knockBackPower = 0.3f;
		attackInfo.damage = 10;

		spPlayer->OnHit(attackInfo);

		m_hitTarget = true;
	}

	m_pDebugWire->AddDebugSphere(sphere.Center, sphere.Radius, kGreenColor);

}

void TurtleShell::OnHit(const AttackInfo attackInfo)
{

	m_hp -= attackInfo.damage;

	if (m_hp <= 0)
	{
		m_hp = 0;
		m_outroFlg = true;
		ChangeState<TurtleShellDieState>();
	}
	else
	{
		if(!IsAttack())
		{
			ChangeState<TurtleShellDamageState>();
			RePlayAnimation(TurtleShellAnimationType::GetHit);
		}
	}

	FlyTextManager::Instance().CreateDamateText(attackInfo.damage, GetPos());

	AddKnockBack(attackInfo.knockBackDir, attackInfo.knockBackPower);
}


void TurtleShell::UpdateAttack()
{
	// ターゲットに到達したら攻撃する
	if (m_hasReachedTarget)
	{
		m_attackFlg = true;
	}

	m_attackCooldown -= m_deltaTime;
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

	auto spPlayer = m_wpPlayer.lock();
	if (spPlayer)
	{
		m_targetPos = spPlayer->GetPos();
	}


	Math::Vector3 moveDir = GetMoveDir();

	if (moveDir.LengthSquared() <= 0.000001f)
	{
		return;
	}

	moveDir.Normalize();


	const auto& param = m_parameter.GetParam().m_moveSpeed;
	Math::Vector3 move = moveDir * (param + 0.07f) * 60.0f * m_deltaTime;


	for (auto wall : WallCollisionManager::Instance().GetWallCollisionList())
	{
		if (!wall)
		{
			continue;
		}

		const auto& box = wall->GetBox();

		Math::Vector3 boxCenter = box.Center;
		Math::Vector3 boxExtents = box.Extents;

		Math::Vector3 boxMin = boxCenter - boxExtents;
		Math::Vector3 boxMax = boxCenter + boxExtents;

		float toi = 0.0f;
		Math::Vector3 normal = Math::Vector3::Zero;

		bool hit = CollisionMath::SphereSweepVsAABB(
			GetBumpSphere().Center,
			GetBumpSphere().Radius,
			move,
			boxMin,
			boxMax,
			toi,
			normal);

		if (!hit)
		{
			continue;
		}

		float dot = moveDir.Dot(normal);

		if (dot < 0.0f)
		{
			Math::Vector3 reflectDir = moveDir - 2.0f * dot * normal;

			if (reflectDir.LengthSquared() > 0.000001f)
			{
				reflectDir.Normalize();
				moveDir = reflectDir;
				SetMoveDir(moveDir);
			}
		}

		break;
	}
	
	move = moveDir * (param + 0.07f) * 60.0f * m_deltaTime;
	AddPendingMove(move);
}

bool TurtleShell::SpinAttackRemaining()
{
	
	m_spinAttackRemaining -= m_deltaTime;

	if (m_spinAttackRemaining <= 0)
	{
		return true;
	}

	return false;
}

bool TurtleShell::DizyyRemaining()
{
	m_dizzyRemaining -= m_deltaTime;

	if (m_dizzyRemaining <= 0)
	{
		return true;
	}

	return false;
}

void TurtleShell::HitCoolDownRemaining()
{
	m_hitCooldownRemaining -= m_deltaTime;

	if (m_hitCooldownRemaining<= 0)
	{
		m_hitCooldownRemaining = m_hitCooldownDuration;
		m_hitTarget = false;
	}
}

void TurtleShell::UpdateAnimation()
{
	m_animation.Update(m_deltaTime);
}