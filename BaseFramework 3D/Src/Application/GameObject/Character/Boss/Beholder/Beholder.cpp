#include "Beholder.h"

#include"../../../../System/CollisionManager/CollisionManager.h"

#include"State/States/BeholderNormalState.h"

void Beholder::Init()
{
	if (!m_spModel)
	{
		InitCharacterModel("Asset/Models/Boss/Beholder/Beholder.gltf", "Beholder",
			Math::Vector3(0, 1.5, 0), 0.5f, "Beholder");

		// アニメーションクラス初期化
		m_animation.Init(m_spModel);
		// パラメータクラス初期化
		m_parameter.Init();

		m_stateMachine.Start(this);
		m_stateMachine.ChangeState<BeholderNormalState>();
	}

	BossBase::Init();

	CollisionManager::Instance().RegisterObject(CollisionLayer::CharacterBump, shared_from_this());

	SetPos({ 0.0f,0.0f,0.0f });

}

void Beholder::Update()
{
	m_stateMachine.Update();
}

void Beholder::PostUpdate()
{
	BossBase::PostUpdate();

	UpdateAnimation();

	DrawBumpDebugSphere(Math::Vector3(0, 1.5, 0), 0.5f);

}

void Beholder::UpdateAnimation()
{
	m_animation.Update(m_deltaTime);
}

void Beholder::PlayAnimation(BeholderAnimationType type)
{
	m_animation.Play(type);
}

void Beholder::DrawParameterInspector()
{
	m_parameter.DrawInspecter();
}
