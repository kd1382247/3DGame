#include "Golem.h"

#include"../../../../System/CollisionManager/CollisionManager.h"

#include"State/States/GolemNormalState.h"

void Golem::Init()
{
	if (!m_spModel)
	{
		// パラメータ初期化(当たり判定の大きさに使うので、モデル生成より先に読み込む)
		m_parameter.Init();

		ApplyParameter();

		InitCharacterModel("Asset/Models/Boss/Golem/Golem.gltf", "Golem", "Golem");

		// アニメーションクラス初期化
		m_animation.Init(m_spModel);

		m_stateMachine.Start(this);
		m_stateMachine.ChangeState<GolemNormalState>();
	}
	BossBase::Init();

	CollisionManager::Instance().RegisterObject(CollisionLayer::CharacterBump, shared_from_this());

	SetPos({ 0.0f,0.0f,0.0f });
	SetScale(m_parameter.GetParam().m_scale);
}

void Golem::Update()
{
	m_stateMachine.Update();
}

void Golem::PostUpdate()
{
	BossBase::PostUpdate();

	UpdateAnimation();

	DrawBumpDebugSphere();

}

void Golem::DrawParameterInspector()
{
	m_parameter.DrawInspecter();

	// 当たり判定球の変更をすぐに反映
	const auto& bumpParam = m_parameter.GetParam();
	SetBumpSphere(bumpParam.m_bumpSphereOffsetY, bumpParam.m_bumpSphereRadius);
}

void Golem::ApplyParameter()
{
	const auto& param = m_parameter.GetParam();

	m_health.Init(param.m_maxHP);

	// スフィアの高さと半径
	m_bumpSphereOffsetY = param.m_bumpSphereOffsetY;
	m_bumpSphereRadius = param.m_bumpSphereRadius;

	// 重力加速度をパラメータから反映
	SetGravityAcceleration(param.m_gravityAcceleration);

	m_reachDistance = param.m_reachDistance;
	m_reachDistanceMargin = param.m_reachDistanceMargin;

}

void Golem::UpdateAnimation()
{

	m_animation.Update(m_deltaTime);

}

void Golem::PlayAnimation(GolemAnimationType type)
{
	m_animation.Play(type);
}
