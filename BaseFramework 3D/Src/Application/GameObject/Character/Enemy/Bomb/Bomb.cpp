#include "Bomb.h"

#include<algorithm>

#include"../../../../System/CollisionManager/CollisionManager.h"

#include"../../../HPBar/EnemyHPBar/EnemyHPBarManager.h"

#include"../../../Explosion/ExplosionManager.h"

#include"State/States/BombNormalState.h"
#include"State/States/BombDamageState.h"
#include"State/States/BombDieState.h"

void Bomb::Init()
{
	if (!m_spModel)
	{
		// パラメータ初期化(当たり判定の大きさに使うので、モデル生成より先に読み込む)
		m_parameter.Init();

		const auto& param = m_parameter.GetParam();

		InitCharacterModel("Asset/Models/Enemy/Bomb/Bomb.gltf", "Bomb",
			Math::Vector3(0.0f, param.m_bumpSphereOffsetY, 0.0f), param.m_bumpSphereRadius, "Bomb");

		// アニメーションクラス初期化
		m_animation.Init(m_spModel);
		m_health.Init(param.m_maxHP);

		// 当たり判定(押し戻し)球の大きさをパラメータから反映
		SetBumpSphere(param.m_bumpSphereOffsetY, param.m_bumpSphereRadius);

		m_reachDistance = param.m_reachDistance;
		m_attackCooldownDuration = param.m_attackCooldown;

		m_explosionRadius = param.m_explosionRadius;
		m_chargeDuration = param.m_chargeDuration;

		m_stateMachine.Start(this);
		m_stateMachine.ChangeState<BombNormalState>();
	}

	EnemyBase::Init();

	CollisionManager::Instance().RegisterObject(CollisionLayer::CharacterBump, shared_from_this());


	SetPos({ 0.0f,0.0f,0.0f });
}

void Bomb::Update()
{

	m_pDebugWire->AddDebugSphere(GetPos(),m_explosionRadius,kRedColor);
	
	UpdateGravity();

	m_stateMachine.Update();

	UpdateAttack();
}

void Bomb::PostUpdate()
{
	EnemyBase::PostUpdate();

	UpdateAnimation();

	DrawBumpDebugSphere();

}

void Bomb::DrawParameterInspector()
{
	m_parameter.DrawInspecter();

	// 当たり判定球の変更をすぐに反映
	const auto& bumpParam = m_parameter.GetParam();
	SetBumpSphere(bumpParam.m_bumpSphereOffsetY, bumpParam.m_bumpSphereRadius);
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
		explosionPos, m_explosionRadius, m_parameter.GetParam().m_attackPow, m_parameter.GetParam().m_explosionKnockBack);

	// 爆発エフェクト
	KdEffekseerManager::GetInstance().
		Play("Enemy/Explosion/Explosion.efkefc", explosionPos, 0.6f, 1.4f, false);

	KdAudioManager::Instance().PlaySE("Asset/Data/Sound/SE/Enemy/Explosion/Explosion.wav");
}

void Bomb::CreateExplosionRange()
{
	// 爆発範囲と予告を表示するカラースフィアを作る(表示中なら何もしない)
	m_rangeIndicator.Show();
	m_telegraphIndicator.Show();
}

void Bomb::UpdateExplosionRange()
{
	// 爆発範囲(爆発の当たり判定と同じ座標・半径)
	m_rangeIndicator.DrawCircle(GetPos(), m_explosionRadius);

	// ため終わったら、予告の円は役目を終える(爆発範囲の表示は、爆発が終わるまで残す)
	if (IsChargeFinished())
	{
		m_telegraphIndicator.Hide();
		return;
	}

	// 予告は、ためが進むにつれて範囲いっぱいまで広がっていく
	// (ため時間が0の場合は、最初から満ちているものとして扱う)
	float progress = (m_chargeDuration > 0.0f) ? (1.0f - (m_chargeTimer / m_chargeDuration)) : 1.0f;
	progress = std::clamp(progress, 0.0f, 1.0f);

	m_telegraphIndicator.DrawCircle(GetPos(), m_explosionRadius * progress);
}

void Bomb::HideExplosionRange()
{
	// 表示を消す(どちらも、消えている場合は何もしない)
	m_rangeIndicator.Hide();
	m_telegraphIndicator.Hide();
}

void Bomb::OnHit(const AttackInfo attackInfo)
{
	if (ApplyDamage(attackInfo))
	{
		m_stateMachine.ChangeState<BombDieState>();
	}
	else if (!IsAttack())
	{
		// 爆発中は怯まない
		m_stateMachine.ChangeState<BombDamageState>();
		RePlayAnimation(BombAnimationType::GetHit);
	}
}

