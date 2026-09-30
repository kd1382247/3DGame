#include "TurtleShell.h"

#include"../../../../System/CollisionManager/CollisionManager.h"
#include"../../../../System/CollisionManager/CollisionMath/CollisionMath.h"
#include"../../../Stage/Collision/AABBCollision/AABBCollisionManager.h"
#include"../../../Stage/Collision/AABBCollision/AABBCollision.h"

#include"../../../Effect/EffectManager.h"

#include"../../../HPBar/EnemyHPBar/EnemyHPBarManager.h"

#include"../../Player/Player.h"

#include"State/States/TurtleShellNormalState.h"
#include"State/States/TurtleShellDamageState.h"
#include"State/States/TurtleShellDieState.h"

void TurtleShell::Init()
{
	if (!m_spModel)
	{
		InitCharacterModel("Asset/Models/Enemy/TurtleShell/TurtleShell.gltf", "TurtleShell",
			Math::Vector3(0.0f, 0.5f, 0.0f), 0.4f, "TurtleShell");

		// アニメーションクラス初期化
		m_animation.Init(m_spModel);

		// パラメータクラス初期化
		m_parameter.Init();

		const auto& param = m_parameter.GetParam();

		m_health.Init(param.m_maxHP);

		m_reachDistance = param.m_reachDistance;
		m_attackCooldownDuration = param.m_attackCooldown;
		m_dizzyDuration = param.m_dizzyDuration;
		m_spinAttackDuration = param.m_spinDuration;
		m_hitCooldownDuration = param.m_spinHitCooldown;

		m_stateMachine.Start(this);
		m_stateMachine.ChangeState<TurtleShellNormalState>();
	}

	EnemyBase::Init();

	CollisionManager::Instance().RegisterObject(CollisionLayer::CharacterBump, shared_from_this());

	SetPos({ 0.0f,0.0f,0.0f });
}

void TurtleShell::Update()
{
	UpdateGravity();

	m_stateMachine.Update();

	UpdateAttack();

}

void TurtleShell::PostUpdate()
{

	UpdateAnimation();

	EnemyBase::PostUpdate();
}

void TurtleShell::DrawDebug()
{
	DrawBumpDebugSphere(Math::Vector3(0.0f, 0.5f, 0.0f), 0.4f);
	//m_pDebugWire->Draw();
}

void TurtleShell::DrawParameterInspector()
{
	m_parameter.DrawInspecter();
}

void TurtleShell::SetUpReference()
{
	EnemyBase::SetUpReference();

	// HPBarを生成
	EnemyHPBarManager::Instance().
		CreateHPBar(shared_from_this(), Math::Vector3(-0.7f, 1.5f, 0.0f));
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

	EffectManager::Instance().CreateEffect("Stun", shared_from_this(),Math::Vector3(0,1.2,0));
}

void TurtleShell::EndDizzy()
{
	m_attackFlg = false;
	m_attackCooldown = m_attackCooldownDuration;
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

	const auto& param = m_parameter.GetParam();

	DirectX::BoundingSphere sphere;

	sphere.Center = GetPos()+Math::Vector3(0.0f,0.5f,0.0f);
	sphere.Radius = param.m_spinHitRadius;

	KdCollider::SphereInfo sphereInfo(KdCollider::TypeBump, sphere);

	if (spPlayer->Intersects(sphereInfo, nullptr))
	{
		AttackPlayer(spPlayer, param.m_knockBackPower, param.m_attackPow);

		m_hitTarget = true;
	}

	m_pDebugWire->AddDebugSphere(sphere.Center, sphere.Radius, kGreenColor);

}

void TurtleShell::OnHit(const AttackInfo attackInfo)
{
	if (ApplyDamage(attackInfo))
	{
		m_stateMachine.ChangeState<TurtleShellDieState>();
	}
	else if (!IsAttack())
	{
		m_stateMachine.ChangeState<TurtleShellDamageState>();
		RePlayAnimation(TurtleShellAnimationType::GetHit);
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


	// 回転中は、通常の移動速度に SpinSpeedBonus を足した速さで動く
	const float spinSpeed = m_parameter.GetParam().m_moveSpeed + m_parameter.GetParam().m_spinSpeedBonus;
	Math::Vector3 move = moveDir * spinSpeed * 60.0f * m_deltaTime;


	for (auto wall : AABBCollisionManager::Instance().GetAABBCollisionList())
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

	move = moveDir * spinSpeed * 60.0f * m_deltaTime;
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

void TurtleShell::PlayWalkAnimation()
{
	PlayAnimation(TurtleShellAnimationType::Walk);
}

void TurtleShell::PlayIdleAnimation()
{
	PlayAnimation(TurtleShellAnimationType::Idle);
}