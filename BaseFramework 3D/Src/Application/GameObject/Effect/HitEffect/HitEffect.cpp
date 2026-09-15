#include "HitEffect.h"

#include"../../Camera/CameraBase.h"

#include"../../../System/GameObjectFinder/GameObjectFinder.h"

#include"../../../System/TimeManager/TimeManager.h"

void HitEffect::Init()
{

	if (!m_spPolygon)
	{
		m_spPolygon = std::make_shared<KdSquarePolygon>();
		m_spPolygon->SetMaterial("Asset/Textures/Effect/HitEffect/HitEffect.png");

		m_spPolygon->SetSplit(16, 1);

		m_spPolygon->SetScale(2);

		SetUpReference();
	}

}

void HitEffect::Update()
{
	auto spTarget = m_wpTarget.lock();

	if (!spTarget)
	{
		Destroy();
		return;
	}

	SetPos(m_offsetPos);

	float deltaTime = TimeManager::Instance().GetDeltaTime();

	m_animCount += (0.5 * 60) * deltaTime;

	m_spPolygon->SetUVRect(static_cast<int>(m_animCount));

	
	if (m_animCount > maxAnimCount)
	{
		Destroy();
	}

	EffectBase::UpdateBillboard();
}

void HitEffect::DrawEffect()
{
	if (!m_spPolygon)
	{
		return;
	}

	KdShaderManager::Instance().ChangeDepthStencilState(KdDepthStencilState::ZWriteDisable);

	KdShaderManager::Instance().m_StandardShader.DrawPolygon(*m_spPolygon, m_mWorld);

	KdShaderManager::Instance().UndoDepthStencilState();
}

void HitEffect::DrawBright()
{
	if (m_spPolygon)
	{
		KdShaderManager::Instance().m_StandardShader.DrawPolygon(*m_spPolygon, m_mWorld);
	}
}

void HitEffect::SetUpReference()
{
	EffectBase::SetUpReference();

	if (!m_wpCamera.lock())
	{
		m_wpCamera = GameObjectFinder::Instance().FindObject<CameraBase>();
	}
}
