#include "MageNovaCircleState.h"

#include"../../Mage.h"
#include"MageNormalState.h"
#include"../../MageMagicCircle/MageMagicCircle.h"

void MageNovaCircleState::OnStart(Mage* mage)
{
	mage->PlayAnimation(MageAnimationType::AreaWideMagic);

	m_castTimer = 0.0f;
	m_phase = Phase::Cast;

	m_param = mage->GetParam().m_novaCircle;
	m_centerPos = mage->GetPos();

	m_rangeIndicator.Show();
	m_telegraphIndicator.Show();

	m_wpMagicCircle.reset();
}

void MageNovaCircleState::OnUpdate(Mage* mage)
{
	m_castTimer += mage->GetDeltaTime();

	switch (m_phase)
	{
	case MageNovaCircleState::Phase::Cast:
		UpdateCast(mage);
		break;
	case MageNovaCircleState::Phase::Active:
		UpdateActive(mage);
		break;
	}

}

void MageNovaCircleState::OnExit(Mage* mage)
{
	// 表示円を消す
	m_rangeIndicator.Hide();
	m_telegraphIndicator.Hide();

	mage->EndAttack();
}

void MageNovaCircleState::UpdateCast(Mage* mage)
{
	// 攻撃範囲を表示
	m_rangeIndicator.DrawCircle(m_centerPos, m_param.m_radius);


	if (m_param.m_telegraph <= 0.0f)
	{
		m_param.m_telegraph = 1.0f;
	}

	// 円の拡大の割合
	float progress = std::clamp(m_castTimer / m_param.m_telegraph, 0.0f, 1.0f);

	// 攻撃のタイミングの円を表示
	m_telegraphIndicator.DrawCircle(m_centerPos, m_param.m_radius * progress);

	// 詠唱のアニメーションが終わったら、StartActiveへ進む
	if (mage->IsAnimationFinished())
	{
		StartActive(mage);
	}

}

void MageNovaCircleState::StartActive(Mage * mage)
{
	// カラースフィアを消す
	m_rangeIndicator.Hide();
	m_telegraphIndicator.Hide();

	m_wpMagicCircle = mage->CastNovaCircle();
	mage->PlayAnimation(MageAnimationType::Idle);

	m_phase = Phase::Active;
}

void MageNovaCircleState::UpdateActive(Mage * mage)
{
	// 魔法円が無くなった(=攻撃終了した)らNormalStateへ戻る
	auto spMagicCircle = m_wpMagicCircle.lock();

	if (!spMagicCircle || spMagicCircle->IsFinished())
	{
		m_pMachine->ChangeState<MageNormalState>();
	}
}
