#include "Golem.h"

#include"../../../../System/CollisionManager/CollisionManager.h"

#include"State/States/GolemNormalState.h"

void Golem::Init()
{
	if (!m_spModel)
	{
		InitCharacterModel("Asset/Models/Boss/Golem/Golem.gltf", "Golem",
			Math::Vector3(0.0f, 0.5f, 0.0f), 0.4f, "Golem");

		// アニメーションクラス初期化
		m_animation.Init(m_spModel);
		// パラメータークラス初期化
		m_parameter.Init();

		m_hp = m_parameter.GetParam().m_maxHP;

		m_stateMachine.Start(this);
		m_stateMachine.ChangeState<GolemNormalState>();
	}
	BossBase::Init();

	CollisionManager::Instance().RegisterObject(CollisionLayer::CharacterBump, shared_from_this());

	SetPos({ 0.0f,0.0f,0.0f });
	SetScale(2.0f);
}

void Golem::Update()
{
	m_stateMachine.Update();
}

void Golem::PostUpdate()
{
	BossBase::PostUpdate();

	UpdateAnimation();

	DrawBumpDebugSphere(Math::Vector3(0.0f, 0.5f, 0.0f), 0.4f);

}

void Golem::DrawParameterInspector()
{
	m_parameter.DrawInspecter();
}

void Golem::UpdateAnimation()
{

	m_animation.Update(m_deltaTime);

}

void Golem::PlayAnimation(GolemAnimationType type)
{
	m_animation.Play(type);
}
