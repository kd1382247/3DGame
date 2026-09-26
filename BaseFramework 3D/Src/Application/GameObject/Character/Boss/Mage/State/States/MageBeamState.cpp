#include "MageBeamState.h"

#include"../../Mage.h"
#include"MageNormalState.h"

#include"../../../../../MageBeam/MageBeam.h"

namespace
{
	// 詠唱開始からビームが発生するまでの時間(秒)
	constexpr float kCastDelay = 0.8f;

	// ビームの長さ・幅(仮値。後で調整する)
	constexpr float kBeamLength = 19.0f;
	constexpr float kBeamWidth = 2.5f;

	// ボス中心ではなく、ボスの前方に押し出す量
	// (テレグラフ表示・当たり判定・エフェクトの基準座標をすべてこの位置に揃える)
	constexpr float kBeamForwardOffset = 7.0;
}

void MageBeamState::OnStart(Mage* mage)
{
	mage->PlayAnimation(MageAnimationType::Attack1);

	// 詠唱開始時点で座標・方向を確定させる(テレグラフ表示とBeam生成で同じ値を使うため)
	m_beamDir = mage->GetMatrix().Backward();
	m_beamPos = mage->GetPos() + m_beamDir * kBeamForwardOffset;

	m_effectPos = (mage->GetPos() + Math::Vector3(0.0f, 0.8f, 0.0f)) + m_beamDir * 1.8f;

	// 予告範囲(矩形)の表示を開始
	CreateBeamRange();
	CreateTelegraphIndicator();
	// 詠唱中の予告(windup)エフェクトを再生
	m_wpEffekseerObj = KdEffekseerManager::GetInstance().Play(
		"Beam/Beam.efkefc", m_effectPos , 1.0f, 1.0f, false, 0, 120, mage->GetBeamEffectRotation());

	m_castTimer = 0.0f;
	m_hasCast = false;
}

void MageBeamState::OnUpdate(Mage* mage)
{
	// 詠唱中・Beam生成後を通して、予告範囲(矩形)を表示し続ける
	UpdateBeamRange();
	UpdateTelegraphIndicator();
	if (!m_hasCast)
	{
		m_castTimer += mage->GetDeltaTime();

		if (m_castTimer >= kCastDelay)
		{
			m_wpMageBeam = mage->FireBeam(m_beamPos, m_beamDir, kBeamLength, kBeamWidth,m_effectPos);
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
			Math::Vector2(kBeamWidth, kBeamLength),
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

		float progress = m_castTimer / kCastDelay;
		Math::Vector2 currentRect = Math::Vector2(kBeamWidth, kBeamLength);
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


