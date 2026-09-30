#include "MageBeamState.h"

#include"../../Mage.h"
#include"MageNormalState.h"

#include"../../../../../MageBeam/MageBeam.h"

void MageBeamState::OnStart(Mage* mage)
{
	mage->PlayAnimation(MageAnimationType::BeamST);

	// ビームの設定をParameterから受け取る(詠唱中は同じ値を使い続ける)
	const auto& param = mage->GetParam();

	m_castDelay = param.m_beamCastDelay;
	m_beamLength = param.m_beamLength;
	m_beamWidth = param.m_beamWidth;

	// 詠唱開始時点で座標・方向を確定させる(テレグラフ表示とBeam生成で同じ値を使うため)
	// ボス中心ではなく、ボスの前方に押し出した位置を基準にする
	// (テレグラフ表示・当たり判定・エフェクトの基準座標をすべてこの位置に揃える)
	m_beamDir = mage->GetMatrix().Backward();
	m_beamPos = mage->GetPos() + m_beamDir * param.m_beamForwardOffset;

	m_effectPos = (mage->GetPos() + Math::Vector3(0.0f, 1.5f, 0.0f)) + m_beamDir * 1.8f;

	// 予告範囲(矩形)の表示を開始
	CreateBeamRange();
	CreateTelegraphIndicator();
	// 詠唱中の予告(windup)エフェクトを再生
	m_wpEffekseerObj = KdEffekseerManager::GetInstance().Play(
		"Beam/Beam.efkefc", m_effectPos , 1.0f, 1.0f, false, 0, 120, mage->GetEffectRotation(180));

	m_castTimer = 0.0f;
	m_hasCast = false;
}

void MageBeamState::OnUpdate(Mage* mage)
{
	// 詠唱中・Beam生成後を通して、予告範囲(矩形)を表示し続ける
	UpdateBeamRange();
	UpdateTelegraphIndicator();

	if (mage->IsAnimationFinished())
	{
		mage->PlayAnimation(MageAnimationType::BeamRPT);
	}

	if (!m_hasCast)
	{
		m_castTimer += mage->GetDeltaTime();

		if (m_castTimer >= m_castDelay)
		{
			m_wpMageBeam = mage->FireBeam(m_beamPos, m_beamDir, m_beamLength, m_beamWidth, m_effectPos);
			m_hasCast = true;
		}

		return;
	}
	else
	{
		HideTelegraphIndicator();
	}

	
	// Beam本体が無くなった(=攻撃終了した)らNormalStateへ戻る
	auto spMageBeam = m_wpMageBeam.lock();

	if (!spMageBeam || spMageBeam->IsFinished())
	{
		m_pMachine->ChangeState<MageNormalState>();
	}
}

void MageBeamState::OnExit(Mage* mage)
{
	HideBeamRange();

	mage->EndAttack();
}

void MageBeamState::CreateBeamRange()
{
	// 攻撃範囲を表示するカラースフィア(矩形)を作る
	m_colorSphereHandle = KdShaderManager::Instance().CreateColorSphere();
}

void MageBeamState::UpdateBeamRange()
{
	if (m_colorSphereHandle != -1)
	{
		KdShaderManager::Instance().WriteCBColorSphereRectangle(
			m_colorSphereHandle,
			m_beamPos,
			m_beamDir,
			Math::Vector2(m_beamWidth, m_beamLength),
			Math::Vector3(2.0f, 0.0f, 0.0f));
	}
}

void MageBeamState::HideBeamRange()
{
	// 表示を消す
	if (m_colorSphereHandle != -1)
	{
		KdShaderManager::Instance().ReleaseColorSphere(m_colorSphereHandle);
		m_colorSphereHandle = -1;
	}

}

void MageBeamState::CreateTelegraphIndicator()
{

	// 攻撃範囲を表示するカラースフィア(矩形)を作る
	m_telegraphIndicatorHandle = KdShaderManager::Instance().CreateColorSphere();
}

void MageBeamState::UpdateTelegraphIndicator()
{
	if (m_telegraphIndicatorHandle != -1)
	{

		// 詠唱時間が0の場合は、最初から予告が満ちているものとして扱う
		float progress = (m_castDelay > 0.0f) ? (m_castTimer / m_castDelay) : 1.0f;
		Math::Vector2 currentRect = Math::Vector2(m_beamWidth, m_beamLength);
		currentRect *= progress;


		KdShaderManager::Instance().WriteCBColorSphereRectangle(
			m_telegraphIndicatorHandle,
			m_beamPos,
			m_beamDir,
			currentRect,
			Math::Vector3(2.0f, 0.0f, 0.0f));
	}
}

void MageBeamState::HideTelegraphIndicator()
{
	// 表示を消す
	if (m_telegraphIndicatorHandle != -1)
	{
		KdShaderManager::Instance().ReleaseColorSphere(m_telegraphIndicatorHandle);
		m_telegraphIndicatorHandle = -1;
	}

}


