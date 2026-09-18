#include "Bomb.h"

#include"../../../../System/CollisionManager/CollisionManager.h"

#include"../../../FlyText/FlyTextManager.h"

#include"../../../HPBar/EnemyHPBar/EnemyHPBarManager.h"

#include"../../Player/Player.h"

#include"../../../Explosion/ExplosionManager.h"

#include"State/States/BombNormalState.h"
#include"State/States/BombDamageState.h"
#include"State/States/BombDieState.h"

void Bomb::Init()
{
	if (!m_spModel)
	{
		InitEnemyModel("Asset/Models/Enemy/Bomb/Bomb.gltf", "Bomb",
			Math::Vector3(0.0f, 0.5f, 0.0f), 0.4f, "Bomb");

		// アニメーションクラス初期化
		m_animation.Init(m_spModel);
		// パラメータ初期化
		m_parameter.Init();

		m_hp = m_parameter.GetParam().m_maxHP;

		m_attackCooldownDuration = 1.0f;

		m_stateMachine.Start(this);
		m_stateMachine.ChangeState<BombNormalState>();
	}

	EnemyBase::Init();

	CollisionManager::Instance().RegisterObject(CollisionLayer::CharacterBump, shared_from_this());


	SetPos({ 0.0f,0.0f,0.0f });
}

void Bomb::Update()
{

	UpdateGravity();

	m_stateMachine.Update();

	UpdateAttack();
}

void Bomb::PostUpdate()
{
	EnemyBase::PostUpdate();

	UpdateAnimation();

	DrawBumpDebugSphere(Math::Vector3(0.0f, 0.5f, 0.0f), 0.4f);

}

void Bomb::DrawParameterInspector()
{
	m_parameter.DrawInspecter();
}

void Bomb::SetUpReference()
{
	EnemyBase::SetUpReference();

	// HPBarを生成
	EnemyHPBarManager::Instance().
		CreateHPBar(shared_from_this(), Math::Vector3(-0.7f, 1.5f, 0.0f));
}

void Bomb::PlayAnimation(BombAnimationType type)
{
	m_animation.Play(type);
}

void Bomb::RePlayAnimation(BombAnimationType type)
{
	m_animation.RePlay(type);
}

void Bomb::UpdateLaunch()
{
	if (IsGrounded())
	{
		m_launchFlg = false;
	}

	Math::Vector3 move = m_launchVec * 60.0f * m_deltaTime;

	AddPendingMove(move);
}

void Bomb::UpdateAnimation()
{
	m_animation.Update(m_deltaTime);
}

void Bomb::PlayWalkAnimation()
{
	PlayAnimation(BombAnimationType::Walk);
}

void Bomb::PlayIdleAnimation()
{
	PlayAnimation(BombAnimationType::Idle);
}

void Bomb::StartCharge()
{
	m_chargeTimer = m_chargeDuration;
}

void Bomb::UpdateCharge()
{
	m_chargeTimer -= m_deltaTime;

	if (m_chargeTimer < 0.0f)
	{
		m_chargeTimer = 0.0f;
	}
}

void Bomb::Explode()
{
	// 爆発の中心は自分の位置(近接攻撃と違い前方には出さない)
	Math::Vector3 explosionPos = GetPos() + Math::Vector3(0.0f, 0.5f, 0.0f);

	// 当たり判定は専用のExplosionオブジェクトに任せる
	// (自分はこの直後に消えてしまうため、自分自身では当たり判定を持てない)
	ExplosionManager::Instance().CreateExplosion(
		explosionPos, m_explosionRadius, m_parameter.GetParam().m_attackPow, 0.5f);

	// 爆発エフェクト
	KdEffekseerManager::GetInstance().
		Play("Explosion/Explosion.efkefc", explosionPos, 0.4f, 1.0f, false);
}

void Bomb::ShowExplosionRange()
{
	// ステージの色を変えて爆発範囲を表示する
	// (当たり判定(Explosion)と同じ半径を使うことで、見た目と実際の範囲を一致させる。
	//  座標は地面の色塗りなので、当たり判定のような高さオフセットは付けず、地面と同じ高さで見る)
	KdShaderManager::Instance().WriteCBColor(GetPos(), m_explosionRadius, Math::Vector3(0.3f, 0.0f, 0.0f));
}

void Bomb::HideExplosionRange()
{
	// 表示を消す(半径0にすることでどのピクセルも範囲内に入らなくなる)
	KdShaderManager::Instance().WriteCBColor(GetPos(), 0.0f, Math::Vector3::Zero);
}

void Bomb::OnHit(const AttackInfo attackInfo)
{
	m_hp -= attackInfo.damage;

	if (m_hp <= 0)
	{
		m_hp = 0;
		m_outroFlg = true;
		m_stateMachine.ChangeState<BombDieState>();
	}
	else if (!IsAttack())
	{
		// 爆発中は怯まない
		m_stateMachine.ChangeState<BombDamageState>();
		RePlayAnimation(BombAnimationType::GetHit);
	}

	FlyTextManager::Instance().CreateDamateText(attackInfo.damage, GetPos());

	KdEffekseerManager::GetInstance().
		Play("Hit/Hit.efkefc", GetPos() + Math::Vector3(0.0f, 0.5f, 0.0f), 0.4f, 1.0f, false);

	AddKnockBack(attackInfo.knockBackDir, attackInfo.knockBackPower);
}
