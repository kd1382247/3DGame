#include "EffectBase.h"

#include"../Camera/CameraBase.h"
#include"../../GameObject/Character/Player/Player.h"

#include"../../System/GameObjectFinder/GameObjectFinder.h"

void EffectBase::Init()
{}

void EffectBase::Update()
{}

void EffectBase::DrawEffect()
{
	if (!m_spPolygon)
	{
		return;
	}

	KdShaderManager::Instance().m_StandardShader.DrawPolygon(*m_spPolygon, m_mWorld);
}

void EffectBase::SetUpReference()
{
	if (!m_wpPlayer.lock())
	{
		m_wpPlayer = GameObjectFinder::Instance().FindObject<Player>();
	}
}

void EffectBase::UpdateBillboard()
{

	auto spCamera = m_wpCamera.lock();
	auto spTarget = m_wpTarget.lock();

	if (!spCamera || !spTarget)
	{
		return;
	}

	// 敵のワールド座標
	const Math::Vector3 targetPos = spTarget->GetPos();

	// カメラのワールド回転行列(view行列ではない)
	const Math::Matrix camRotMat = spCamera->GetRotationMatrix();

	// GetPos() は「カメラ基準のオフセット」として扱う。
	// -Z がカメラ側になる座標系を想定。
	const Math::Vector3 camLocalOffset = GetPos();


	const Math::Vector3 worldOffset = Math::Vector3::TransformNormal(camLocalOffset, camRotMat);

	const Math::Vector3 effectPos = targetPos + worldOffset;

	// 向きはカメラに合わせ、位置は計算済みの座標にする
	m_mWorld =camRotMat *Math::Matrix::CreateTranslation(effectPos);

}
