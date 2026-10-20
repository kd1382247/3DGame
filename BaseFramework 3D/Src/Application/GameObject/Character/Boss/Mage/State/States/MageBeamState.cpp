#include "MageBeamState.h"

#include<algorithm>

#include"../../Mage.h"
#include"MageNormalState.h"

#include"../../MageBeam/MageBeam.h"

void MageBeamState::OnStart(Mage* mage)
{
	mage->PlayAnimation(MageAnimationType::BeamChant);

	// ビームの設定をParameterから受け取る(詠唱中は同じ値を使い続ける)
	m_param = mage->GetParam().m_beam;

	// 詠唱開始時点で座標・方向を確定させる(テレグラフ表示とBeam生成で同じ値を使うため)
	mage->GetBeamPose(m_param, m_beamPos, m_beamDir);

	// 攻撃範囲(矩形)・攻撃予測の表示を始める
	m_rangeIndicator.Show();
	m_telegraphIndicator.Show();

	// 詠唱中の予告(windup)エフェクトを再生
	m_wpEffekseerObj = mage->PlayAttackEffect(m_param.m_windupEffect);

	m_castTimer = 0.0f;
	m_phase = Phase::Cast;
}

void MageBeamState::OnUpdate(Mage* mage)
{
	// 詠唱中・Beam生成後を通して、攻撃範囲(矩形)を表示し続ける
	m_rangeIndicator.DrawRectangle(m_beamPos, m_beamDir, Math::Vector2(m_param.m_width, m_param.m_length));


	switch (m_phase)
	{
	case Phase::Cast:
		UpdateCast(mage);
		break;

	case Phase::Active:
		UpdateActive(mage);
		break;

	case Phase::End:
		UpdateEnd(mage);
		break;
	}
}

void MageBeamState::OnExit(Mage* mage)
{
	// 詠唱の途中で別のステートへ切り替わっても、範囲・予告の表示とエフェクトが残らないようにする
	m_rangeIndicator.Hide();
	m_telegraphIndicator.Hide();

	if (auto spEffect = m_wpEffekseerObj.lock())
	{
		spEffect->StopEffect();
	}

	mage->EndAttack();
}

void MageBeamState::UpdateCast(Mage* mage)
{
	m_castTimer += mage->GetDeltaTime();

	if (m_castTimer >= m_param.m_castDelay)
	{
		StartActive(mage);

		return;
	}

	// 詠唱時間が0の場合は、最初から予告が満ちているものとして扱う
	float progress = (m_param.m_castDelay > 0.0f) ? (m_castTimer / m_param.m_castDelay) : 1.0f;
	progress = std::clamp(progress, 0.0f, 1.0f);

	// 予告は、詠唱が進むにつれて範囲いっぱいまで広がっていく
	m_telegraphIndicator.DrawRectangle(
		m_beamPos,
		m_beamDir,
		Math::Vector2(m_param.m_width, m_param.m_length) * progress);
}

void MageBeamState::StartActive(Mage* mage)
{
	m_wpMageBeam = mage->FireBeam(m_param);

	// 予告の役目は終わり(攻撃範囲の表示は、ビームが終わるまで残す)
	m_telegraphIndicator.Hide();

	m_phase = Phase::Active;

	mage->PlayAnimation(MageAnimationType::BeamRPT);

}

void MageBeamState::UpdateActive(Mage* mage)
{
	// Beam本体が無くなったらEndフェーズへ
	auto spMageBeam = m_wpMageBeam.lock();


	if (!spMageBeam || spMageBeam->IsFinished())
	{
		m_phase = Phase::End;
		m_rangeIndicator.Hide();

		if (auto spEffect = m_wpEffekseerObj.lock())
		{
			spEffect->StopEffect();
		}
	}
}

void MageBeamState::UpdateEnd(Mage* mage)
{
	mage->PlayAnimation(MageAnimationType::BeamEND);

	if (mage->IsAnimationFinished())
	{
		m_pMachine->ChangeState<MageNormalState>();
	}
}
