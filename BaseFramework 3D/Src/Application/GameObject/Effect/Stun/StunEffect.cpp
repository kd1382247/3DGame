#include "StunEffect.h"

#include"../../Camera/CameraBase.h"

#include"../../Character/CharacterBase.h"

#include"../../../System/GameObjectFinder/GameObjectFinder.h"

#include"../../../System/TimeManager/TimeManager.h"

void StunEffect::Init()
{

	if (!m_spPolygon)
	{
		m_spPolygon = std::make_shared<KdSquarePolygon>();
		m_spPolygon->SetMaterial("Asset/Textures/Effect/Stun/Stun.png");

		m_spPolygon->SetSplit(8,1);

		m_spPolygon->SetScale(2);

		SetUpReference();
	}

}

void StunEffect::Update()
{

	// キャラベースに変換
	auto spTarget= std::dynamic_pointer_cast<CharacterBase>(m_wpTarget.lock());

	if (!spTarget||spTarget->IsInOutro())
	{
		Destroy();
		return;
	}
	
	SetPos(m_offsetPos);

	float deltaTime = TimeManager::Instance().GetDeltaTime();

	m_animCount += (0.2*60)*deltaTime;

	m_spPolygon->SetUVRect(static_cast<int>(m_animCount));

	m_effectDuration -= 60 * deltaTime;

	if (m_animCount > maxAnimCount)
	{
		m_animCount = 0.0f;
	}

	if (m_effectDuration<=0.0f)
	{
		Destroy();
	}

	EffectBase::UpdateBillboard();
}

void StunEffect::DrawEffect()
{
	if (!m_spPolygon)
	{
		return;
	}

	KdShaderManager::Instance().ChangeDepthStencilState(KdDepthStencilState::ZWriteDisable);
	
	KdShaderManager::Instance().m_StandardShader.DrawPolygon(*m_spPolygon, m_mWorld);
	
	KdShaderManager::Instance().UndoDepthStencilState();
}

void StunEffect::SetUpReference()
{
	if (!m_wpCamera.lock())
	{
		m_wpCamera = GameObjectFinder::Instance().FindObject<CameraBase>();
	}
}
