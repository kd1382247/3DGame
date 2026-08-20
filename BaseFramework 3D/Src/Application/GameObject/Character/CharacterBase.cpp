#include "CharacterBase.h"

#include"../../System/CollisionManager/CollisionManager.h"

CharacterBase::CharacterBase()
{}

CharacterBase::~CharacterBase()
{
	Release();
}

// 初期化
void CharacterBase::Init()
{}

// 更新
void CharacterBase::Update()
{}

void CharacterBase::PostUpdate()
{
	
}

// 描画
void CharacterBase::DrawLit()
{
	if (m_spModel)
	{
		KdShaderManager::Instance().
			m_StandardShader.DrawModel(*m_spModel, m_mWorld);
	}
}

void CharacterBase::GenerateDepthMapFromLight()
{
	if (m_spModel)
	{
		KdShaderManager::Instance().
			m_StandardShader.DrawModel(*m_spModel, m_mWorld);
	}
}

void CharacterBase::DrawInspector()
{
	DrawBasicInspecter();

	ImGui::Separator();

	ImGui::Text("----------Parameter----------");
}

DirectX::BoundingSphere CharacterBase::GetBumpSphere() const
{
	DirectX::BoundingSphere sphere;

	sphere.Center = GetPos() + Math::Vector3(0, 0.5, 0);
	sphere.Radius = 0.5;

	return sphere;
}

void CharacterBase::UpdateCollision()
{
	//UpdateCharacterBumpCollision();
	//UpdateWallCollision();
	//UpdateGroundCollision();

	//UpdateWallCollision();
}

// 解放
void CharacterBase::Release()
{
	m_spModel = nullptr;
}