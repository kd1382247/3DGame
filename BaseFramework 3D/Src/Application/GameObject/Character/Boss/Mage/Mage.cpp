#include "Mage.h"

#include"../../../../System/CollisionManager/CollisionManager.h"

#include"State/States/MageNormalState.h"

void Mage::Init()
{
	if (!m_spModel)
	{
		InitCharacterModel("Asset/Models/Boss/Mage/Mage.gltf", "Mage",
			Math::Vector3(0.0f, 0.5f, 0.0f), 0.4f, "Mage");

		// アニメーションクラス初期化
		m_animation.Init(m_spModel);
		// パラメータークラス初期化
		m_parameter.Init();

		m_stateMachine.Start(this);
		m_stateMachine.ChangeState<MageNormalState>();
	}

	BossBase::Init();

	CollisionManager::Instance().RegisterObject(CollisionLayer::CharacterBump, shared_from_this());


	SetPos({ 0.0f,0.0f,0.0f });
	SetScale(1.5f);
}

void Mage::Update()
{
	m_stateMachine.Update();
}

void Mage::PostUpdate()
{
	BossBase::PostUpdate();

	UpdateAnimation();

	DrawBumpDebugSphere(Math::Vector3(0.0f, 0.5f, 0.0f), 0.4f);

}

void Mage::DrawParameterInspector()
{
	m_parameter.DrawInspecter();
}

void Mage::UpdateAnimation()
{
	m_animation.Update(m_deltaTime);
}

void Mage::PlayAnimation(MageAnimationType type)
{
	m_animation.Play(type);
}
